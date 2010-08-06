//
//  VideoDetailViewController.m
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/2/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import "MediaDetailViewController.h"


@implementation MediaDetailViewController

@synthesize thumbnailView, uploadButton, viewOriginalButton, viewProcessedButton, imageZoomController;
@synthesize statusLabel, typeLabel, mediaIDLabel;

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


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad 
{
	appDelegate =	[[UIApplication sharedApplication] delegate];
	prefs =			[NSUserDefaults standardUserDefaults];
	deviceUDID =	[[[UIDevice currentDevice] uniqueIdentifier] retain];
	deviceName =	[[[UIDevice currentDevice] name] retain];
	fileManager =	[[NSFileManager alloc] init];
	
	documentsDir = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask,YES) objectAtIndex:0] retain];
	mediaDir = [[documentsDir stringByAppendingPathComponent:@"originals"] retain];
	unlogoDir = [[documentsDir stringByAppendingPathComponent:@"unlogo"] retain];
	thumbnailsDir = [[documentsDir stringByAppendingPathComponent:@"thumbnails"] retain];
	
	
    [super viewDidLoad];
}


- (void) setActiveItem:(NSDictionary*)item 
{
	currentItem = item;
	
	NSString* status = [currentItem objectForKey:@"status"];
	NSString* type = [currentItem objectForKey:@"type"];
	
	self.title = [currentItem objectForKey:@"title"];
	statusLabel.text = status;
	typeLabel.text = type;
	mediaIDLabel.text = [currentItem objectForKey:@"media_id"];
	thumbnailView.image = [UIImage imageWithContentsOfFile:[currentItem objectForKey:@"thumbnail"]];


	[uploadButton setEnabled:[status isEqualToString:@"new"]];
	[viewProcessedButton setEnabled:[status isEqualToString:@"done"]];
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
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
	
	thumbnailView = nil;
	uploadButton = nil;
	viewOriginalButton = nil;
	statusLabel = nil;
	typeLabel = nil;
}


- (void)dealloc {
	if(uploadProgressAlert!=nil)
	{
		[uploadProgressAlert release];
	}
	if(downloadProgressAlert!=nil)
	{
		[downloadProgressAlert release];
	}
	[thumbnailView release];
	[uploadButton release];
	[viewOriginalButton release];
	[statusLabel release];
	[typeLabel release];
    [super dealloc];
}


#pragma mark -
#pragma mark ButtonActions

- (IBAction)viewProcessed
{	
	NSLog(@"View Processed");
	NSString* unlogo = [currentItem objectForKey:@"unlogo"];
	if(unlogo==nil || ![fileManager fileExistsAtPath:unlogo]) 
	{
		NSLog(@"Unlogo doesn't exist.  Starting download.");
		[self doDownload];
	} 
	else 
	{
		NSString* type = [currentItem objectForKey:@"type"];
		if ([type isEqualToString:@"image"])
		{
			[imageZoomController setPhoto:unlogo];
			[[self parentViewController] pushViewController:imageZoomController animated:YES];
		}
		else if ([type isEqualToString:@"video"])
		{		
			CustomMoviePlayerViewController	*moviePlayer = [[[CustomMoviePlayerViewController alloc] initWithPath:unlogo] autorelease];
			[self presentModalViewController:moviePlayer animated:YES];
			[moviePlayer readyPlayer]; 
		}
	}
}

- (void)doDownload
{
	NSString* download_link = [currentItem objectForKey:@"download_link"];
	if(download_link==nil)
	{
		UIAlertView *alert = [[UIAlertView alloc] 
							  initWithTitle:@"Oops" 
							  message:@"This video is not yet ready for download"
							  delegate:self
							  cancelButtonTitle:@"Ok" 
							  otherButtonTitles:nil];
		[alert show];
		[alert release];
		return;
	}
	
	NSURL* url = [NSURL URLWithString:download_link];
	NSString* dest = [[unlogoDir stringByAppendingPathComponent:[url lastPathComponent]] retain];
	NSLog(@"downloading %@ to %@", url, dest);

	[currentItem setValue:dest forKey:@"unlogo"];
	
	ASIHTTPRequest *request = [ASIHTTPRequest requestWithURL:url];
	[request setDownloadDestinationPath:dest];
	[request setShowAccurateProgress:YES];
	[request setDelegate:self];
	[request setDidFinishSelector:@selector(downloadDone:)];
	[request setDidFailSelector:@selector(downloadWentWrong:)];
	
	downloadProgressAlert = [[UIAlertView alloc] initWithTitle: @"Downloading... Please wait."
												message: @" "
												delegate: self
												cancelButtonTitle: @"Cancel"
												otherButtonTitles: nil];
	[downloadProgressAlert setTag:ALERT_DOWNLOAD_CANCELLED];
	
	UIProgressView *progressView = [[UIProgressView alloc] initWithFrame:CGRectMake(20.0f, 50.0f, 250.0f, 90.0f)];
	
	[downloadProgressAlert addSubview:progressView];
	
	[request setUploadProgressDelegate:progressView];
	[request startAsynchronous];
	
	[progressView release];
	[downloadProgressAlert show];
}


- (IBAction)viewOriginal:(id)sender
{
	NSString* path = [currentItem objectForKey:@"original"];
	NSString* type = [currentItem objectForKey:@"type"];
	if(path == NULL)
	{
		NSString* message = [NSString stringWithFormat:@"Can't find %@!", path];
		UIAlertView *alert = [[UIAlertView alloc] 
							  initWithTitle:@"Can't Find Video!" 
							  message:message
							  delegate:self
							  cancelButtonTitle:@"Ok" 
							  otherButtonTitles:nil];
		
		[alert setTag:ALERT_VIDEO_MISSING];
		[alert show];
		[alert release];
	}
	else if ([type isEqualToString:@"image"])
	{
		[imageZoomController setPhoto:path];
		[[self parentViewController] pushViewController:imageZoomController animated:YES];
	}
	else if ([type isEqualToString:@"video"])
	{
		NSLog(@"Attempting to play %@", path);
		
		// Create custom movie player   
		CustomMoviePlayerViewController	*moviePlayer = [[[CustomMoviePlayerViewController alloc] initWithPath:path] autorelease];
		
		// Show the movie player as modal
		[self presentModalViewController:moviePlayer animated:YES];
		
		// Prep and play the movie
		[moviePlayer readyPlayer]; 
	}
}

/*
- (IBAction)dismissZoomView:(id)sender
{
	[self dismissModalViewControllerAnimated:YES];	
}
*/


- (IBAction)doUpload:(id)sender
{
	NSString* original = [currentItem objectForKey:@"original"];
	NSString* type = [currentItem objectForKey:@"type"];
	NSString* media_id = [currentItem objectForKey:@"media_id"];
	NSString* title = [currentItem objectForKey:@"title"];
	
	ASIFormDataRequest *request = [[ASIFormDataRequest alloc] initWithURL:appDelegate.endpoint];
	
	if ([type isEqualToString:@"image"])
	{
		[request setData:[NSData dataWithContentsOfFile:original] withFileName:@"image.jpg" andContentType:@"image/jpeg" forKey:@"file"];
	}
	else if ([type isEqualToString:@"video"])
	{
		[request setFile:original forKey:@"file"];
		[request setShouldStreamPostDataFromDisk:YES];
	}
	
	[request setPostValue:@"upload" forKey:@"action"];
	[request setPostValue:deviceUDID forKey:@"udid"];
	[request setPostValue:deviceName forKey:@"device_name"];
	[request setData:appDelegate.deviceToken withFileName:@"token.bin" andContentType:@"application/octet-stream" forKey:@"device_token"];
	[request setPostValue:media_id forKey:@"media_id"];
	[request setPostValue:title forKey:@"title"];
	[request setShowAccurateProgress:YES];
	[request setDelegate:self];
	[request setDidFinishSelector:@selector(uploadDone:)];
	[request setDidFailSelector:@selector(uploadWentWrong:)];
	[request setNumberOfTimesToRetryOnTimeout:4];
	
	uploadProgressAlert = [[UIAlertView alloc] initWithTitle: @"Uploading... Please wait."
													 message: @" "
													delegate: self
										   cancelButtonTitle: @"Cancel"
										   otherButtonTitles: nil];
	[uploadProgressAlert setTag:ALERT_UPLOAD_CANCELLED];
	
	UIProgressView *progressView = [[UIProgressView alloc] initWithFrame:CGRectMake(20.0f, 50.0f, 250.0f, 90.0f)];
	
	[uploadProgressAlert addSubview:progressView];
	
	
	[request setUploadProgressDelegate:progressView];
	[request startAsynchronous];
	
	[progressView release];
	[uploadProgressAlert show];
}



#pragma mark -
#pragma mark HTTPRequestCallbacks

- (void)downloadDone:(ASIHTTPRequest *)request
{
	[downloadProgressAlert dismissWithClickedButtonIndex:1 animated:YES];
	NSLog(@"Download done.  Viewing %@", [currentItem objectForKey:@"unlogo"]);
	[self viewProcessed];
}

- (void)downloadWentWrong:(ASIHTTPRequest *)request 
{
	[downloadProgressAlert dismissWithClickedButtonIndex:0 animated:YES];
	
	NSError *error = [request error];
	//[statusLabel setText:@""];
	NSLog(@"Request Failed: %@", [error localizedDescription] );
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[error localizedDescription]
													message: [error localizedFailureReason]
												   delegate:self 
										  cancelButtonTitle:@"Cancel" 
										  otherButtonTitles:@"Retry", nil];
	[alert setTag:ALERT_DOWNLOAD_FAILED];
	[alert show];
	[alert release];
	[request release];
}

//--------------------------------------------------------------
- (void)uploadDone:(ASIHTTPRequest *)request {
	
	[uploadProgressAlert dismissWithClickedButtonIndex:1 animated:YES];
	
	NSString *responseString = [request responseString];
	NSLog(@"Response: %@", responseString);
	
	NSDictionary *dictionary = [responseString JSONValue];
	
	NSLog(@"!!!  Make sure this is a valid JSON string! ");
	
	
	NSString* statusString = [dictionary objectForKey:@"status"];

	if([statusString isEqualToString:@"ok"])
	{	
		UIAlertView *alert = [[UIAlertView alloc] 
							  initWithTitle:@"Upload Successful!" 
							  message:@"Your upload was successful.  You will be notified when your upload has been processed." 
							  delegate:self cancelButtonTitle:@"Ok" 
							  otherButtonTitles:nil];
		
		[alert setTag:ALERT_UPLOAD_SUCCEEDED];
		[alert show];
		[alert release];
		
		[currentItem setValue:@"uploaded" forKey:@"status"];
	}
	
	if([statusString isEqualToString:@"error"])
	{
		
		NSMutableString *alertMessage = [[NSMutableString alloc] initWithString:@"There was an error on the server.\n"];
		NSArray* errors = [dictionary objectForKey:@"errors"];
		
		NSDictionary *error;
		NSString *message, *code;
		int errorCount = [errors count];
		for (int i = 0; i < errorCount; i++) {
			error = [errors objectAtIndex:i];
			message = [error objectForKey:@"message"];
			code = [error objectForKey:@"code"];
			if([code isEqualToString:@"UPLOAD_FILE_EXISTS"])
			{
				[currentItem setValue:@"uploaded" forKey:@"status"];
				[uploadButton setHidden:YES];
			}
			
			[alertMessage appendString: message];
			[alertMessage appendString:@"\n"];
		}
		
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Server-side Error" 
									message:alertMessage
									delegate:self cancelButtonTitle:@"Ok" 
									otherButtonTitles:nil];
		
		[alert setTag:ALERT_UPLOAD_SERVER_ERROR];
		[alert show];
		[alert release];
	}

	[request release];
}


//--------------------------------------------------------------
- (void)uploadWentWrong:(ASIHTTPRequest *)request
{
	[uploadProgressAlert dismissWithClickedButtonIndex:0 animated:YES];
	
	NSError *error = [request error];
	//[statusLabel setText:@""];
	NSLog(@"Request Failed: %@", [error localizedDescription] );
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[error localizedDescription]
													message: [error localizedFailureReason]
													delegate:self 
													cancelButtonTitle:@"Cancel" 
													otherButtonTitles:@"Retry", nil];
	[alert setTag:ALERT_UPLOAD_FAILED];
	[alert show];
	[alert release];
	[request release];
}



#pragma mark -
#pragma mark UIAlertViewDelegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex{
	
	switch([alertView tag]) {
			
		case ALERT_UPLOAD_FAILED:
			if (buttonIndex == 1)
			{
				NSLog(@"retry upload");
				[self doUpload:nil];
			}
			break;
		case ALERT_DOWNLOAD_FAILED:
			if (buttonIndex == 1)
			{
				NSLog(@"retry download");
				[self doDownload];
			}
			break;
		case ALERT_VIDEO_MISSING:
			break;
		case ALERT_UPLOAD_CANCELLED:
			[uploadProgressAlert dismissWithClickedButtonIndex:0 animated:YES];
			[self.navigationController popViewControllerAnimated:YES];
			break;
		case ALERT_UPLOAD_SERVER_ERROR:
			break;
		case ALERT_UPLOAD_SUCCEEDED:
			[self.navigationController popViewControllerAnimated:YES];
			break;
		default:
			NSLog(@"WARNING:  No handler for alert with ID %d", [alertView tag]);
			break;
	}
}


@end
