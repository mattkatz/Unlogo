//
//  UnlogoAppDelegate.h
//  Unlogo
//
//  Created by Jeffrey Crouse on 7/27/10.
//  Copyright Eyebeam 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UnlogoAppDelegate : NSObject <UIApplicationDelegate> {
    
    UIWindow							*window;
    UINavigationController				*navigationController;
	NSData								*deviceToken;
	NSURL								*endpoint;

}

@property (nonatomic, readonly) NSURL *endpoint;
@property (nonatomic, readonly) NSData *deviceToken;
@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UINavigationController *navigationController;

@end

