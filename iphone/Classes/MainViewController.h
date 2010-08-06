//
//  MainViewController.h
//  Unlogo
//
//  Created by Jeffrey Crouse on 7/27/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "UnlogoAppDelegate.h"
#import "MediaDetailViewController.h"
#import "ASIHTTPRequest.h"
#import "ASIFormDataRequest.h"
#import "SBJSON.h"
#import "IPAddress.h"
#import "Preferences.h"


#define ALERT_SAVING_VIDEO_FAILED 1
#define ALERT_SAVING_IMAGE_FAILED 2
#define ALERT_SYNC_SERVER_ERROR 3
#define ALERT_SYNC_FAILED 4

@interface MainViewController : UIViewController <UINavigationBarDelegate,
												UIAlertViewDelegate,
												UITableViewDelegate,
												UITableViewDataSource,
												UIActionSheetDelegate,
												UINavigationControllerDelegate,
												UIImagePickerControllerDelegate,
												ASIHTTPRequestDelegate>
{
	
	UnlogoAppDelegate					*appDelegate;	// reference to the main app delegate
	Preferences							*appPrefs;
	
	
	NSFileManager						*fileManager;
	
	UITableView							*myTableView;
	UIViewController					*aboutView, *settingsView;;				
	MediaDetailViewController			*videoDetailView;
	
	NSString							*deviceUDID, *deviceName;
	NSString							*ipAddress;
	
	BOOL								usingLibraryMedia;
	
	NSString							*documentsDir, *mediaDir, *unlogoDir, *thumbnailsDir;
}

@property (nonatomic, retain) IBOutlet UITableView					*myTableView;
@property (nonatomic, retain) IBOutlet UIViewController				*aboutView;
@property (nonatomic, retain) IBOutlet UIViewController				*settingsView;
@property (nonatomic, retain) IBOutlet MediaDetailViewController	*videoDetailView;


- (IBAction)newItemAction:(id)sender;
- (IBAction)showAboutView:(id)sender;
- (IBAction)showSettingsView:(id)sender;
- (IBAction)synchronizeWithServer;

- (NSString*) generateMediaID;
- (BOOL)mkdir:(NSString*)path;
- (BOOL)getItemByPath:(NSString*)path  item:(NSDictionary**)result;
- (BOOL)getItemByMediaID:(NSString*)media_id  item:(NSDictionary**)result;


// ASIHttpRequest callbacks
- (void)syncWentWrong:(ASIHTTPRequest *)request;
- (void)syncDone:(ASIHTTPRequest *)request;

//SaveToAlbum callbacks
- (void) image:(UIImage *)image didFinishSavingWithError: (NSError *)error contextInfo:(void *)contextInfo;
- (void) video:(NSString *)videoPath didFinishSavingWithError: (NSError *)error contextInfo:(void *)contextInfo;

// UIImagePickerController delagate
- (void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info;
- (void) imagePickerControllerDidCancel:(UIImagePickerController *)picker;
- (void) navigationController:(UINavigationController *)navigationController didShowViewController:(UIViewController *)viewController animated:(BOOL)animated;
- (void) navigationController:(UINavigationController *)navigationController willShowViewController:(UIViewController *)viewController animated:(BOOL)animated;


+ (UIImage*)imageWithImage:(UIImage*)sourceImage scaledToSizeWithSameAspectRatio:(CGSize)targetSize;
+ (NSString*)getThumbnailPathForVideo:(NSString *)vid;
+ (NSString*)deviceIPAdress;

@end
