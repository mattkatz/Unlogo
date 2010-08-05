//
//  VideoDetailViewController.h
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/2/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import "UnlogoAppDelegate.h"
#import "ASIHTTPRequest.h"
#import "ASIFormDataRequest.h"
#import "SBJSON.h"
#import "CustomMoviePlayerViewController.h"
#import "ImageZoomController.h"


#define ALERT_UPLOAD_FAILED 1
#define ALERT_UPLOAD_SUCCEEDED 2
#define ALERT_UPLOAD_CANCELLED 3
#define ALERT_UPLOAD_SERVER_ERROR 4
#define ALERT_VIDEO_MISSING 5

@interface MediaDetailViewController : UIViewController <UIAlertViewDelegate,ASIHTTPRequestDelegate> {
	
	UnlogoAppDelegate					*appDelegate;	// reference to the main app delegate
	NSUserDefaults						*prefs;
	NSFileManager						*fileManager;
	
	UIAlertView							*uploadProgressAlert;
	UIImageView							*thumbnailView;
	UILabel								*statusLabel, *typeLabel, *mediaIDLabel;
	UIButton							*uploadButton, *viewOriginalButton, *viewProcessedButton;
	
	NSString							*deviceUDID;
	NSString							*deviceName;

	NSDictionary						*currentItem;
	
	CustomMoviePlayerViewController		*moviePlayer;
	ImageZoomController					*imageZoomController;
}

@property (nonatomic, retain) IBOutlet	UIImageView			*thumbnailView;
@property (nonatomic, retain) IBOutlet	UIButton			*uploadButton, *viewOriginalButton, *viewProcessedButton;
@property (nonatomic, retain) IBOutlet	UILabel				*statusLabel, *typeLabel, *mediaIDLabel;
@property (nonatomic, retain) IBOutlet  ImageZoomController	*imageZoomController;


// HTTPRequest callbacks
- (void) uploadDone:(ASIHTTPRequest *)request;
- (void) uploadWentWrong:(ASIHTTPRequest *)request;

//- (IBAction)dismissZoomView:(id)sender;
- (IBAction)doUpload:(id)sender;
- (IBAction)viewOriginal:(id)sender;
- (void) setActiveItem:(NSDictionary*) item;
- (void) viewProcessed:(id)sender;
@end
