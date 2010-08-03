//
//  VideoDetailViewController.m
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/2/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import "MediaDetailViewController.h"


@implementation MediaDetailViewController

@synthesize thumbnailView, statusLabel, typeLabel, uploadButton, viewOriginalButton, imageZoomController;


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

    [super viewDidLoad];
}


- (void) setActiveItem:(NSDictionary*) item 
{
	currentItem = item;
	
	NSString* status = [currentItem objectForKey:@"status"];
	NSString* type = [currentItem objectForKey:@"type"];
	
	self.title = [currentItem objectForKey:@"title"];
	statusLabel.text = status;
	typeLabel.text = type;
	thumbnailView.image = [UIImage imageWithContentsOfFile:[currentItem objectForKey:@"thumbnail"]];

	if(![status isEqualToString:@"new"])
	{
		[uploadButton setHidden:YES];
	}
	
}

- (IBAction)viewOriginal:(id)sender
{
	NSString* path = [currentItem objectForKey:@"filename"];
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
	else if ([type isEqualToString:@"photo"])
	{
		[imageZoomController setPhoto:path];
		[[self parentViewController] pushViewController:imageZoomController animated:YES];
	}
	else if ([type isEqualToString:@"video"])
	{
		NSLog(@"Attempting to play %@", path);
		
		// Create custom movie player   
		moviePlayer = [[[CustomMoviePlayerViewController alloc] initWithPath:path] autorelease];
		
		// Show the movie player as modal
		[self presentModalViewController:moviePlayer animated:YES];
		
		// Prep and play the movie
		[moviePlayer readyPlayer]; 
	}
}


- (IBAction)dismissZoomView:(id)sender
{
	[self dismissModalViewControllerAnimated:YES];	
}

- (IBAction)doUpload:(id)sender
{
	NSString* path = [currentItem objectForKey:@"filename"];
	NSString* type = [currentItem objectForKey:@"type"];
	
	ASIFormDataRequest *uploadRequest = [[ASIFormDataRequest alloc] initWithURL:appDelegate.endpoint];
	
	if ([type isEqualToString:@"photo"])
	{
		[uploadRequest setData:[NSData dataWithContentsOfFile:path] withFileName:@"image.jpg" andContentType:@"image/jpeg" forKey:@"file"];
	}
	else if ([type isEqualToString:@"video"])
	{
		[uploadRequest setFile:path forKey:@"file"];
		[uploadRequest setShouldStreamPostDataFromDisk:YES];
	}

	[uploadRequest setShowAccurateProgress:YES];
	[uploadRequest setDelegate:self];
	[uploadRequest setDidFinishSelector:@selector(uploadDone:)];
	[uploadRequest setDidFailSelector:@selector(uploadWentWrong:)];
	[uploadRequest setPostValue:@"upload" forKey:@"action"];
	[uploadRequest setPostValue:deviceUDID forKey:@"udid"];
	[uploadRequest setPostValue:deviceName forKey:@"device_name"];
	[uploadRequest setData:appDelegate.deviceToken withFileName:@"token.bin" andContentType:@"application/octet-stream" forKey:@"device_token"];
	
	uploadProgressAlert = [[UIAlertView alloc] initWithTitle: @"Uploading... Please wait."
							message: @" "
							delegate: self
							cancelButtonTitle: @"Cancel"
							otherButtonTitles: nil];
	[uploadProgressAlert setTag:ALERT_UPLOAD_CANCELLED];
	
	UIProgressView *progressView = [[UIProgressView alloc] initWithFrame:CGRectMake(20.0f, 50.0f, 250.0f, 90.0f)];
	
	[uploadProgressAlert addSubview:progressView];
	
	
	[uploadRequest setUploadProgressDelegate:progressView];
	[uploadRequest startAsynchronous];
	
	[progressView release];
	[uploadProgressAlert show];
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
	[thumbnailView release];
	[uploadButton release];
	[viewOriginalButton release];
	[statusLabel release];
	[typeLabel release];
    [super dealloc];
}



#pragma mark -
#pragma mark HTTPRequestCallbacks

//--------------------------------------------------------------
- (void)uploadDone:(ASIHTTPRequest *)request {
	
	[uploadProgressAlert dismissWithClickedButtonIndex:1 animated:YES];
	
	NSString *responseString = [request responseString];
	NSLog(@"Response: %@", responseString);
	
	NSDictionary *dictionary = [responseString JSONValue];
	NSString* statusString = [dictionary objectForKey:@"status"];
	//[statusLabel setText:statusString];
	
	if([statusString isEqualToString:@"ok"]) {	
		UIAlertView *alert = [[UIAlertView alloc] 
							  initWithTitle:@"Upload Successful!" 
							  message:@"Your upload was successful.  You will be notified when your upload has been processed." 
							  delegate:self cancelButtonTitle:@"Ok" 
							  otherButtonTitles:nil];
		
		[alert setTag:ALERT_UPLOAD_SUCCEEDED];
		[alert show];
		[alert release];
		
		[currentItem setValue:@"processing" forKey:@"status"];
	}
	
	if([statusString isEqualToString:@"error"])
	{
		
		NSMutableString *alertMessage = [[NSMutableString alloc] initWithString:@"There was an error on the server.\n"];
		NSArray* errors = [dictionary objectForKey:@"errors"];
		int errorCount = [errors count];
		for (int i = 0; i < errorCount; i++) {
			[alertMessage appendString: [errors objectAtIndex:i]];
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
	
	
	
	//Use when fetching binary data
	//NSData *responseData = [request responseData];
	[request release];
}


//--------------------------------------------------------------
- (void)uploadWentWrong:(ASIHTTPRequest *)request {
	
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
			if (buttonIndex == 0)
			{
				NSLog(@"cancel");
			}
			else
			{
				NSLog(@"retry");
				[self doUpload:nil];
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
