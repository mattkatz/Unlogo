//
//  UnlogoAppDelegate.m
//  Unlogo
//
//  Created by Jeffrey Crouse on 7/27/10.
//  Copyright Eyebeam 2010. All rights reserved.
//

#import "UnlogoAppDelegate.h"
#import "RootViewController.h"


@implementation UnlogoAppDelegate

@synthesize window;
@synthesize navigationController;
@synthesize deviceToken;
@synthesize endpoint;

#pragma mark -
#pragma mark Application lifecycle


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    
    // Override point for customization after application launch.
	NSLog(@"Initiating remoteNoticationssAreActive"); 
    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:(UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound |UIRemoteNotificationTypeAlert)]; 
	
	NSLog(@"Assigning endpoint");
	endpoint = [[NSURL URLWithString:@"http://unlogo.org/gateway/iphone.php"] retain];
	
    // Add the navigation controller's view to the window and display.
    [window addSubview:navigationController.view];
    [window makeKeyAndVisible];

    return YES;
}

- (void)application:(UIApplication *)app didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)_deviceToken {  
	deviceToken =  _deviceToken;
	[deviceToken retain];
	NSLog(@"devToken=%@",deviceToken);
	[[navigationController topViewController] synchronizeWithServer];
}

- (void)application:(UIApplication *)app didFailToRegisterForRemoteNotificationsWithError:(NSError *)err {
    deviceToken = [NSData data]; //empty
    NSLog(@"Error in registration. Error: %@", err);
}

- (void)applicationWillResignActive:(UIApplication *)application {
	NSLog(@"applicationWillResignActive");
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
	NSLog(@"applicationDidEnterBackground");
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
	NSLog(@"applicationWillEnterForeground");
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	NSLog(@"applicationDidBecomeActive");
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}


- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc {
	[navigationController release];
	[window release];
	[super dealloc];
}


@end

