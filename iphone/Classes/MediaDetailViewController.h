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
#define ALERT_DOWNLOAD_CANCELLED 6
#define ALERT_DOWNLOAD_FAILED 7

@interface MediaDetailViewController : UIViewController <UIAlertViewDelegate,ASIHTTPRequestDelegate> {
	
	UnlogoAppDelegate					*appDelegate;	// reference to the main app delegate
	NSUserDefaults						*prefs;
	NSFileManager						*fileManager;
	
	UIAlertView							*uploadProgressAlert, *downloadProgressAlert;
	UIImageView							*thumbnailView;
	UILabel								*statusLabel, *typeLabel, *mediaIDLabel;
	UIBarButtonItem						*uploadButton, *viewOriginalButton, *viewProcessedButton;
	
	NSString							*deviceUDID;
	NSString							*deviceName;

	NSDictionary						*currentItem;
	
	ImageZoomController					*imageZoomController;
	
	UITabBar							*tabBar;
	
	NSString							*documentsDir, *mediaDir, *unlogoDir, *thumbnailsDir;
}

@property (nonatomic, retain) IBOutlet  UITabBar			*tabBar;
@property (nonatomic, retain) IBOutlet	UIImageView			*thumbnailView;
@property (nonatomic, retain) IBOutlet	UIBarButtonItem		*uploadButton, *viewOriginalButton, *viewProcessedButton;
@property (nonatomic, retain) IBOutlet	UILabel				*statusLabel, *typeLabel, *mediaIDLabel;
@property (nonatomic, retain) IBOutlet  ImageZoomController	*imageZoomController;


// HTTPRequest callbacks
- (void) uploadDone:(ASIHTTPRequest *)request;
- (void) uploadWentWrong:(ASIHTTPRequest *)request;
- (void) downloadDone:(ASIHTTPRequest *)request;
- (void) downloadWentWrong:(ASIHTTPRequest *)request;

// API
- (IBAction)doUpload:(id)sender;
- (void)doDownload;
- (IBAction) viewOriginal:(id)sender;
- (IBAction) viewProcessed;
- (void) setActiveItem:(NSDictionary*) item;

@end
