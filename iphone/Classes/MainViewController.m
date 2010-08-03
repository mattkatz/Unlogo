    //
//  MainViewController.m
//  Unlogo
//
//  Created by Jeffrey Crouse on 7/27/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import "MainViewController.h"


@implementation MainViewController

@synthesize myTableView, aboutView, settingsView, videoDetailView;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/


- (void)viewWillAppear:(BOOL)animated
{
	NSLog(@"viewWillAppear");
	[self.navigationController setToolbarHidden:NO animated:YES];
	[self.myTableView deselectRowAtIndexPath:self.myTableView.indexPathForSelectedRow animated:NO];
	
	[self.myTableView reloadData];
}


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	
	// Initialize some shit.
	self.title = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleName"];
	prefs = [NSUserDefaults standardUserDefaults];
	menuList = [[NSMutableArray alloc] init];
	deviceUDID = [[[UIDevice currentDevice] uniqueIdentifier] retain];
	deviceName = [[[UIDevice currentDevice] name] retain];
	fileManager = [[NSFileManager alloc] init];
	appDelegate = [[UIApplication sharedApplication] delegate];
	documentsDirectory = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask,YES) objectAtIndex:0] retain];
	NSLog(@"udid: %@, name: %@", deviceUDID, deviceName);

	// Make the Thumbnail directory if it doesn't exist.
	NSString* thumbnailsDirectory = [documentsDirectory stringByAppendingPathComponent:@"thumbnails"];
	if(![fileManager fileExistsAtPath:thumbnailsDirectory isDirectory:nil]) {
		NSError* err;
		if(![fileManager createDirectoryAtPath:thumbnailsDirectory withIntermediateDirectories:YES attributes:nil error:&err]) {
			NSLog(@"Err desc-%@", [err localizedDescription]);
			NSLog(@"Err reason-%@", [err localizedFailureReason]);
		}
	}
	
	// First try to load the "media" objects from the user prefs
	[self loadTableFromUserPrefs];
	
	// Also look in the documents directory for any stragglers
	[self scanDocumentsDirectory];

	
    [super viewDidLoad];
}


- (void) synchronizeWithServer 
{
	ASIFormDataRequest *request = [[[ASIFormDataRequest alloc] initWithURL:appDelegate.endpoint] retain];
	[request setDelegate:self];
	[request setPostValue:deviceUDID forKey:@"udid"];
	[request setPostValue:@"sync" forKey:@"action"];
	[request setPostValue:deviceName forKey:@"device_name"];
	[request setData:appDelegate.deviceToken withFileName:@"token.bin" andContentType:@"application/octet-stream" forKey:@"device_token"];
	[request setDidFinishSelector:@selector(syncDone:)];
	[request setDidFailSelector:@selector(syncWentWrong:)];
	[request startAsynchronous];	
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	
	NSLog(@"View Unloading!");
	
	// Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
	self.myTableView = nil;
	self.aboutView = nil;
	self.settingsView = nil;
	self.videoDetailView = nil;
    [super viewDidUnload];
}


- (void)dealloc {
	NSLog(@"dealloc");
	[myTableView release];
	[menuList release];
	[aboutView release];
	[videoDetailView release];
	[settingsView release];
    [super dealloc];
}


#pragma mark -
#pragma mark UITableViewDataSource

//--------------------------------------------------------------
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return menuList.count;
}

//--------------------------------------------------------------
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	static NSString *CellIdentifier = @"Cell";
	UITableViewCell *cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
	
	int i = indexPath.row;
	NSDictionary* entry = [menuList objectAtIndex:i];
	NSString* thumbnail = [entry objectForKey:@"thumbnail"];
	if([fileManager fileExistsAtPath:thumbnail])
	{
		UIImageView *imageView = [[UIImageView alloc] initWithImage:[[UIImage alloc] initWithContentsOfFile:thumbnail]];
		cell.accessoryView = imageView;
		[imageView release];
	}
	else
	{
		NSLog(@"Silly rabbit -- %@ doesn't exist.", thumbnail);
	}
	
	if(![fileManager fileExistsAtPath:[entry objectForKey:@"filename"]]) {
		NSLog(@"We've got problems.  %@ doesn't exist.", [entry objectForKey:@"filename"]);
	}
	

	cell.textLabel.lineBreakMode = UILineBreakModeWordWrap;
	cell.textLabel.numberOfLines = 3;
	cell.textLabel.font = [UIFont fontWithName:@"TrebuchetMS-Bold" size:17.0];
	cell.textLabel.text = [NSString stringWithFormat:@"%@\n%@\nstatus: %@", 
						   [entry objectForKey:@"title"], 
						   [entry objectForKey:@"shortName"],
						   [entry objectForKey:@"status"]];
	return cell;
}


#pragma mark -
#pragma mark UITableViewDelegate

//--------------------------------------------------------------
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	[videoDetailView setActiveItem: [menuList objectAtIndex: indexPath.row]];
	[self.navigationController pushViewController:videoDetailView animated:YES];
	[self.navigationController setToolbarHidden:YES animated:YES];
}


- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 80;
}




#pragma mark -
#pragma mark UIActionSheetDelegate

//--------------------------------------------------------------
- (void)actionSheet:(UIActionSheet *)modalView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    // Change the navigation bar style, also make the status bar match with it
	switch (buttonIndex)
	{
		case 0: // Camera
		{
			NSLog(@"opening UIImagePickerController with camera");
			usingLibraryMedia = NO;
			
			UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
			ipc.sourceType =  UIImagePickerControllerSourceTypeCamera;
			ipc.mediaTypes = [UIImagePickerController availableMediaTypesForSourceType:ipc.sourceType];     
			ipc.delegate = self;
			ipc.videoQuality = UIImagePickerControllerQualityTypeHigh;
			[[[UIApplication sharedApplication] keyWindow] addSubview:ipc.view];
			break;
		}
		case 1:  // Library
		{
			NSLog(@"opening UIImagePickerController with library");
			usingLibraryMedia = YES;
			
			UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
			ipc.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
			ipc.mediaTypes = [UIImagePickerController availableMediaTypesForSourceType:ipc.sourceType];     
			ipc.delegate = self;
			[[[UIApplication sharedApplication] keyWindow] addSubview:ipc.view];
			break;
		}
	}
}


#pragma mark -
#pragma mark UIImagePickerControllerDelegate

//--------------------------------------------------------------
- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
	
	NSLog(@"didFinishPickingMediaWithInfo");
	
	NSURL			*mediaURL	= [info objectForKey:UIImagePickerControllerMediaURL];
	NSString		*mediaType	= [info objectForKey:UIImagePickerControllerMediaType];
	NSError			*err;
	NSArray			*keys, *objects;	// This will be put into the menuList array and appear in the table.
	
	if ([mediaType isEqualToString:@"public.image"])
	{
		NSLog(@"image found.");
		UIImage *image = [info objectForKey:UIImagePickerControllerOriginalImage]; 
		NSData* imageData = UIImageJPEGRepresentation(image, 75);

		// Save the image to the documents directory.
		NSString* imageName = [[NSString alloc] initWithFormat:@"image_%d.jpg", menuList.count];
		NSString* fullPathToImage = [documentsDirectory stringByAppendingPathComponent:imageName];
		NSLog(@"Saving %@ to disk", fullPathToImage);
		if([imageData writeToFile:fullPathToImage options:NSDataWritingAtomic error:&err]) {
			NSLog(@"Successfully saved image %@ to file.", fullPathToImage);
		} else {
			NSLog(@"Error:  Could not write %@ to file.", fullPathToImage);
			NSLog(@"Err desc-%@", [err localizedDescription]);
			NSLog(@"Err reason-%@", [err localizedFailureReason]);
			return;
		}
		
		// Make and save a tiny thumbnail image
		UIImage* thumb = [MainViewController imageWithImage:image scaledToSizeWithSameAspectRatio:CGSizeMake(70, 70)];
		NSData* thumbData = UIImageJPEGRepresentation(thumb, 70);
		NSString* thumbName = [[NSString alloc] initWithFormat:@"thumbnails/%@", imageName];
		NSString* fullPathToThumb = [documentsDirectory stringByAppendingPathComponent:thumbName];
		if([thumbData writeToFile:fullPathToThumb atomically:NO]) {
			NSLog(@"Saved thumbnail to %@.", fullPathToThumb);
		} else {
			NSLog(@"Error:  Could not write %@ to file.", fullPathToThumb);
			NSLog(@"Err desc-%@", [err localizedDescription]);
			NSLog(@"Err reason-%@", [err localizedFailureReason]);
		}

		// Add the media to the menuList, which populates the tableView
		NSString* title = [[imageName stringByDeletingPathExtension] stringByReplacingOccurrencesOfString:@"_" withString:@" "];
		keys = [NSArray arrayWithObjects:@"filename", @"type", @"thumbnail", @"shortName", @"title", @"status", nil];
		objects = [NSArray arrayWithObjects:fullPathToImage, @"photo", fullPathToThumb, imageName, title, @"new", nil];


		// Save to album if the user recorded a new video
		if(!usingLibraryMedia)
		{
			NSLog(@"Saving image to album.");
			UIImage* savedImage = [[UIImage alloc] initWithContentsOfFile:fullPathToImage];
			UIImageWriteToSavedPhotosAlbum(savedImage, self, @selector(image:didFinishSavingWithError:contextInfo:), nil);
		}
	}
	else if ([mediaType isEqualToString:@"public.movie"])
	{
		NSLog(@"video found: %@", [mediaURL path]);
		NSString *videoFilePath	= [[mediaURL path] retain];

		// Move the video file to a better location
		// We want to move it regardless of whether it is from the album or not because 
		// the iOS makes a compressed/cropped copy of the file anyway
		NSString* shortName = [[NSString alloc] initWithFormat:@"video_%d.mov", menuList.count];
		NSString* newPath = [documentsDirectory stringByAppendingPathComponent:shortName];
		
		if([fileManager copyItemAtPath:videoFilePath toPath:newPath error:&err])
		{
			NSLog(@"Moved %@ to %@", videoFilePath, newPath);
		}
		else
		{
			NSLog(@"Error:  Could not copy %@ to %@.", videoFilePath, newPath);
			NSLog(@"Err desc-%@", [err localizedDescription]);
			NSLog(@"Err reason-%@", [err localizedFailureReason]);
		}
		
		// Save to album if the user recorded a new video
		UIImage* viewThumb;
		if(!usingLibraryMedia)
		{
			// Find the system-made thumbnail
			NSString *thumbPath = [MainViewController getThumbnailPathForVideo:videoFilePath];
			NSLog(@"Found a video thumbnail at %@", thumbPath);
			viewThumb = [[[UIImage alloc] initWithContentsOfFile:thumbPath] retain];
			
			// Save the video to the album
			if (UIVideoAtPathIsCompatibleWithSavedPhotosAlbum(newPath)){
				NSLog(@"Saving video to album.");
				UISaveVideoAtPathToSavedPhotosAlbum(newPath, self, @selector(video:didFinishSavingWithError:contextInfo:), newPath);
			} else {
				// This is very unlikely as the video is coming directly from the camera and not from the library.
				NSLog(@"video at path is not compatible with saved photos album");
			}
		}
		else
		{
			// Make a stupid little thumbnail image for the upload screen
			CGSize sixzevid=CGSizeMake(picker.view.bounds.size.width,picker.view.bounds.size.height-100);
			UIGraphicsBeginImageContext(sixzevid);
			[picker.view.layer renderInContext:UIGraphicsGetCurrentContext()];
			viewThumb = UIGraphicsGetImageFromCurrentImageContext();
			UIGraphicsEndImageContext();
		}

		UIImage* smallThumb = [MainViewController imageWithImage:viewThumb scaledToSizeWithSameAspectRatio:CGSizeMake(70, 70)];
		NSData* thumbData = UIImageJPEGRepresentation(smallThumb, 70);
		NSString* thumbName = [[NSString alloc] initWithFormat:@"thumbnails/%@.jpg", [shortName stringByDeletingPathExtension]];
		NSString* fullPathToThumb = [documentsDirectory stringByAppendingPathComponent:thumbName];

		if([thumbData writeToFile:fullPathToThumb options:NSDataWritingAtomic error:&err]) {
			NSLog(@"Successfully saved %@ to disk", fullPathToThumb);
		} else {
			NSLog(@"Couldn't write %@ to disk", fullPathToThumb);
			NSLog(@"Err desc-%@", [err localizedDescription]);
			NSLog(@"Err reason-%@", [err localizedFailureReason]);
		}
		
		// Add the media to the menuList, which populates the tableView
		NSString* title = [[shortName stringByDeletingPathExtension] stringByReplacingOccurrencesOfString:@"_" withString:@" "];
		keys = [NSArray arrayWithObjects:@"filename", @"type", @"shortName", @"thumbnail", @"title", @"status", nil];
		objects = [NSArray arrayWithObjects: newPath, @"video", shortName, fullPathToThumb, title, @"new", nil];
	}
	else
	{
		NSLog(@"Media not recognized!");
	}

	// Get rid of the UIImagePickerController
	[picker.view removeFromSuperview];
	[picker release];

	// Add it to the tableView
	NSDictionary* newItem = [NSDictionary dictionaryWithObjects:objects forKeys:keys];
	[menuList addObject:newItem];
	[self.myTableView reloadData];
	
	// Set up the videoDetailView and push the view
	[videoDetailView setActiveItem:newItem];
	[self.navigationController setToolbarHidden:YES animated:YES];
	[self.navigationController pushViewController:videoDetailView animated:YES];
	
	// Set the User Prefs to the updated menuList
	[prefs setObject:menuList forKey:@"media"];
	[prefs synchronize];
	
	
	//NSLog(@"NSUserDefaults dump: %@", [[NSUserDefaults standardUserDefaults] dictionaryRepresentation]);
}

- (void) imagePickerControllerDidCancel:(UIImagePickerController *)picker {
	[picker dismissModalViewControllerAnimated:YES];
	[picker.view removeFromSuperview];
	[picker release];
}


#pragma mark -
#pragma mark SaveToAlbum callbacks

//--------------------------------------------------------------
- (void) video:(NSString *)videoPath didFinishSavingWithError: (NSError *)error contextInfo:(void *)contextInfo {

	if (error != NULL)
    {
		NSLog(@"Error saving video!");	
		UIAlertView *alert = [[UIAlertView alloc] 
							  initWithTitle:[error localizedDescription] 
							  message:[error localizedFailureReason] 
							  delegate:self 
							  cancelButtonTitle:@"Ok" 
							  otherButtonTitles:nil];
		[alert setTag:ALERT_SAVING_VIDEO_FAILED];
		[alert show];
		[alert release];
    }
    else  // No errors
    {
		NSLog(@"Finished saving to album: %@", videoPath);
    }
}

//--------------------------------------------------------------
- (void) image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo {

	if (error != NULL)
    {
		NSLog(@"Error saving image to album!");
		UIAlertView *alert = [[UIAlertView alloc] 
							  initWithTitle:[error localizedDescription] 
							  message:[error localizedFailureReason]
							  delegate:self
							  cancelButtonTitle:@"Ok" 
							  otherButtonTitles:nil];
		[alert setTag:ALERT_SAVING_IMAGE_FAILED];
		[alert show];
		[alert release];
    }
    else  // No errors
    {
		NSLog(@"Finished saving image to album!");
    }
}


#pragma mark -
#pragma mark HTTPRequestCallbacks


//--------------------------------------------------------------
- (void)syncDone:(ASIHTTPRequest *)request {

	NSString *responseString = [request responseString];
	NSLog(@"Sync Response: %@", responseString);
	
	NSDictionary *dictionary = [responseString JSONValue];
	if(![dictionary isKindOfClass:[NSDictionary class]])
	{
		NSLog(@"Server response wasn't a valid JSON object");
		return;
	}
	NSString* statusString = [dictionary objectForKey:@"status"];
	
	if([statusString isEqualToString:@"ok"]) {	
		
		/*
		NSArray *media = [dictionary objectForKey:@"media"];
		
		[prefs setObject:media forKey:@"media"];
		[prefs synchronize];
		
		for (int i = 0; i < [media count]; i++) {
			NSDictionary* item = [media objectAtIndex:i];
			NSString* filename = [item objectForKey:@"filename"];
			NSLog(@"filename: %@", filename);
		}
		 */
	}
	
	if([statusString isEqualToString:@"error"]){
		
		NSMutableString *alertMessage = [[NSMutableString alloc] initWithString:@"There was an error on the server.\n"];
		NSArray* errors = [dictionary objectForKey:@"errors"];
		int errorCount = [errors count];
		for (int i = 0; i < errorCount; i++) {
			[alertMessage appendString: [errors objectAtIndex:i]];
			[alertMessage appendString:@"\n"];
		}
		
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Server-Side Error" 
												message:alertMessage
												delegate:self cancelButtonTitle:@"Ok" 
											  otherButtonTitles:nil];
		
		[alert setTag:ALERT_SYNC_SERVER_ERROR];
		[alert show];
		[alert release];
	}
	
	[request release];
}


//--------------------------------------------------------------
- (void)syncWentWrong:(ASIHTTPRequest *)request {
	NSError *error = [request error];
	NSLog(@"Request Failed: %@", [error localizedDescription] );
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Server Error" 
							message:[error localizedDescription] 
							delegate:self 
							cancelButtonTitle:@"Cancel" 
							otherButtonTitles:@"Retry", nil];
	[alert setTag:ALERT_SYNC_FAILED];
	[alert show];
	[alert release];
	[request release];
}



#pragma mark -
#pragma mark UINavigationControllerDelegate

- (void) navigationController:(UINavigationController *)navigationController didShowViewController:(UIViewController *)viewController animated:(BOOL)animated {
	/*
	 Sent to the receiver just after the navigation controller displays a view controller’s view and navigation item properties.
	 */
}

- (void) navigationController:(UINavigationController *)navigationController willShowViewController:(UIViewController *)viewController animated:(BOOL)animated {
	/*
	 Sent to the receiver just before the navigation controller displays a view controller’s view and navigation item properties.
	 */
}


#pragma mark -
#pragma mark UIAlertViewDelegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
	
	switch([alertView tag]) {
		case ALERT_SAVING_VIDEO_FAILED:
		case ALERT_SAVING_IMAGE_FAILED:
			NSLog(@"ALERT_SAVING_VIDEO_FAILED or ALERT_SAVING_IMAGE_FAILED");
			[self.navigationController popViewControllerAnimated:YES];
			break;
		case ALERT_SYNC_FAILED:
			NSLog(@"ALERT_SYNC_FAILED");
			break;
		case ALERT_SYNC_SERVER_ERROR:
			NSLog(@"ALERT_SYNC_SERVER_ERROR");
			break;
		default:
			NSLog(@"WARNING:  No handler for alert with ID %d", [alertView tag]);
			break;
	}
}


#pragma mark -
#pragma mark API

- (IBAction)newItemAction:(id)sender
{
	UIActionSheet *newItemAlert = [[UIActionSheet alloc] 
								initWithTitle:@"Choose a Source:"
								delegate:self 
								cancelButtonTitle:@"Cancel"
								destructiveButtonTitle:nil
								otherButtonTitles: @"Camera", @"Library", nil, nil];
	
	// use the same style as the nav bar
	//newItemAlert.actionSheetStyle = self.navigationController.navigationBar.barStyle;
	
	[newItemAlert showInView:self.view];
	[newItemAlert release];
}

- (IBAction)showAboutView:(id)sender
{
	[self.navigationController pushViewController:aboutView animated:YES];
	[self.navigationController setToolbarHidden:YES animated:YES];
}

- (IBAction)showSettingsView:(id)sender
{
	NSLog(@"Show Settings");
	[self.navigationController pushViewController:settingsView animated:YES];
	[self.navigationController setToolbarHidden:YES animated:YES];
}


#pragma mark -
#pragma mark Static Methods

+ (UIImage*)imageWithImage:(UIImage*)sourceImage scaledToSizeWithSameAspectRatio:(CGSize)targetSize;
{  
    CGSize imageSize = sourceImage.size;
    CGFloat width = imageSize.width;
    CGFloat height = imageSize.height;
    CGFloat targetWidth = targetSize.width;
    CGFloat targetHeight = targetSize.height;
    CGFloat scaleFactor = 0.0;
    CGFloat scaledWidth = targetWidth;
    CGFloat scaledHeight = targetHeight;
    CGPoint thumbnailPoint = CGPointMake(0.0,0.0);
	
    if (CGSizeEqualToSize(imageSize, targetSize) == NO) {
        CGFloat widthFactor = targetWidth / width;
        CGFloat heightFactor = targetHeight / height;
		
        if (widthFactor > heightFactor) {
            scaleFactor = widthFactor; // scale to fit height
        }
        else {
            scaleFactor = heightFactor; // scale to fit width
        }
		
        scaledWidth  = width * scaleFactor;
        scaledHeight = height * scaleFactor;
		
        // center the image
        if (widthFactor > heightFactor) {
            thumbnailPoint.y = (targetHeight - scaledHeight) * 0.5; 
        }
        else if (widthFactor < heightFactor) {
            thumbnailPoint.x = (targetWidth - scaledWidth) * 0.5;
        }
    }     
	
    CGImageRef imageRef = [sourceImage CGImage];
    CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(imageRef);
    CGColorSpaceRef colorSpaceInfo = CGImageGetColorSpace(imageRef);
	
    if (bitmapInfo == kCGImageAlphaNone) {
        bitmapInfo = kCGImageAlphaNoneSkipLast;
    }
	
    CGContextRef bitmap;
	
    if (sourceImage.imageOrientation == UIImageOrientationUp || sourceImage.imageOrientation == UIImageOrientationDown) {
        bitmap = CGBitmapContextCreate(NULL, targetWidth, targetHeight, CGImageGetBitsPerComponent(imageRef), CGImageGetBytesPerRow(imageRef), colorSpaceInfo, bitmapInfo);
		
    } else {
        bitmap = CGBitmapContextCreate(NULL, targetHeight, targetWidth, CGImageGetBitsPerComponent(imageRef), CGImageGetBytesPerRow(imageRef), colorSpaceInfo, bitmapInfo);
		
    }   
	
    // In the right or left cases, we need to switch scaledWidth and scaledHeight,
    // and also the thumbnail point
    if (sourceImage.imageOrientation == UIImageOrientationLeft) {
        thumbnailPoint = CGPointMake(thumbnailPoint.y, thumbnailPoint.x);
        CGFloat oldScaledWidth = scaledWidth;
        scaledWidth = scaledHeight;
        scaledHeight = oldScaledWidth;
		
        CGContextRotateCTM (bitmap, 90.0/57.2958);
        CGContextTranslateCTM (bitmap, 0, -targetHeight);
		
    } else if (sourceImage.imageOrientation == UIImageOrientationRight) {
        thumbnailPoint = CGPointMake(thumbnailPoint.y, thumbnailPoint.x);
        CGFloat oldScaledWidth = scaledWidth;
        scaledWidth = scaledHeight;
        scaledHeight = oldScaledWidth;
		
        CGContextRotateCTM (bitmap, -90./57.2958);
        CGContextTranslateCTM (bitmap, -targetWidth, 0);
		
    } else if (sourceImage.imageOrientation == UIImageOrientationUp) {
        // NOTHING
    } else if (sourceImage.imageOrientation == UIImageOrientationDown) {
        CGContextTranslateCTM (bitmap, targetWidth, targetHeight);
        CGContextRotateCTM (bitmap, -180./57.2958);
    }
	
    CGContextDrawImage(bitmap, CGRectMake(thumbnailPoint.x, thumbnailPoint.y, scaledWidth, scaledHeight), imageRef);
    CGImageRef ref = CGBitmapContextCreateImage(bitmap);
    UIImage* newImage = [UIImage imageWithCGImage:ref];
	
    CGContextRelease(bitmap);
    CGImageRelease(ref);
	
    return newImage; 
}

+ (NSString*)getThumbnailPathForVideo:(NSString *)vid
{
	NSString *file, *latestFile;
	NSDate* latestDate = [NSDate distantPast];
	NSString* tmpPath = [[vid stringByDeletingLastPathComponent] stringByDeletingLastPathComponent];
	NSDirectoryEnumerator* dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:tmpPath];

	while (file = [dirEnum nextObject]) {
		if ([[file pathExtension] isEqualToString: @"jpg"]) {
			if ([(NSDate *)[[dirEnum fileAttributes] valueForKey:@"NSFileModificationDate"] compare:latestDate] == NSOrderedDescending){
				latestDate = [[dirEnum fileAttributes] valueForKey:@"NSFileModificationDate"];
				latestFile = file;
			}
		}
	}
	
	return [NSTemporaryDirectory() stringByAppendingPathComponent:latestFile];	
}

-(BOOL) fileExistsInTable:(NSString*)filename
{
	NSDictionary* item;
	NSString* other;
	for (int i = 0; i < [menuList count]; i++) {
		item = [menuList objectAtIndex:i];
		other = [item objectForKey:@"filename"];
		if([filename isEqualToString:other]) {
			return YES;
		}
	}
	return NO;
}

- (void) scanDocumentsDirectory
{

	NSError*		err;
	NSArray*		theFilesArray = [fileManager contentsOfDirectoryAtPath:documentsDirectory error:&err];
	
	if (err != NULL)
	{
		
		BOOL isDir, fileExists, alreadyInTable;
		NSString *filename, *fullPath, *baseFilename, *fullPathToThumb, *title;
		for(int i = 0; i<[theFilesArray count]; i++)
		{
			filename = [theFilesArray objectAtIndex: i];
			fullPath = [documentsDirectory stringByAppendingPathComponent:filename];
			fileExists = [fileManager fileExistsAtPath:fullPath isDirectory:&isDir];
			alreadyInTable = [self fileExistsInTable: fullPath];
			baseFilename = [filename stringByDeletingPathExtension];
			fullPathToThumb = [NSString stringWithFormat:@"%@/thumbnails/%@.jpg", documentsDirectory, baseFilename]; 
			title = [[baseFilename stringByDeletingPathExtension] stringByReplacingOccurrencesOfString:@"_" withString:@" "];
			
			if(!alreadyInTable && fileExists && !isDir)
			{
				NSLog(@"scanDocumentsDirectory adding: %@", fullPath);
				if ([[filename pathExtension] isEqualToString: @"mov"])
				{
					// Add the media to the menuList, which populates the tableView
					NSArray *keys = [NSArray arrayWithObjects:@"filename", @"type", @"shortName", @"thumbnail", @"title", @"status", nil];
					NSArray *objects = [NSArray arrayWithObjects: fullPath, @"video", filename, fullPathToThumb, title, @"unknown", nil];
					NSDictionary *dictionary = [NSDictionary dictionaryWithObjects:objects forKeys:keys];
					[menuList addObject:dictionary];
				}
				
				if ([[filename pathExtension] isEqualToString: @"jpg"])
				{
					// Add the media to the menuList, which populates the tableView
					NSArray *keys = [NSArray arrayWithObjects:@"filename", @"type", @"shortName", @"thumbnail", @"title", @"status", nil];
					NSArray *objects = [NSArray arrayWithObjects: fullPath, @"photo", filename, fullPathToThumb, title, @"unknown", nil];
					NSDictionary *dictionary = [NSDictionary dictionaryWithObjects:objects forKeys:keys];
					[menuList addObject:dictionary];
				}
			}
			else
			{
				NSLog(@"scanDocumentsDirectory Skipping: %@", fullPath);
			}
		}
		[self.myTableView reloadData];
	}
	else
	{
		NSLog(@"WARNING:  Couldn't get contents of documents directory!");
	}

}

- (void) loadTableFromUserPrefs
{
	// Try to load up previously-added videos
	NSArray	*media = [prefs objectForKey:@"media"];

	
	if(media != nil)
	{
		NSDictionary* item;
		NSLog(@"Media found! %d items. Loading from defaults.", [media count]);
		
		BOOL alreadyInTable, isDir, fileExists;
		NSString* fullPath;
		
		for (int i = 0; i < [media count]; i++)
		{
			item = [media objectAtIndex:i];
			fullPath = [item objectForKey:@"filename"];
			alreadyInTable = [self fileExistsInTable: fullPath];
			fileExists = [fileManager fileExistsAtPath:fullPath isDirectory:&isDir];
			
			if(fileExists && !alreadyInTable && !isDir)
			{
				NSLog(@"loadTableFromUserPrefs adding %@", fullPath);
				[menuList addObject:item];
			}
			else
			{
				NSLog(@"loadTableFromUserPrefs Skipping: %@", fullPath);
			}
		}
	}
	[self.myTableView reloadData];
}

@end
