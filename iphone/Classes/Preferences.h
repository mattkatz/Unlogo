//
//  Preferences.h
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/4/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface Preferences : NSObject {
	NSMutableDictionary *prefsDict;
	NSString *prefsFilePath;
}

@property (nonatomic,retain) NSMutableDictionary *prefsDict;
@property (nonatomic,retain) NSString *prefsFilePath;

+ (Preferences*)sharedInstance;
- (void)savePrefs;
- (void)loadPrefs;
- (void)initPrefsFilePath;


// Unlogo-specific stuff
- (NSDictionary*)	getMediaItemWithID:(NSString*)media_id;
- (NSDictionary*)	getMediaItemAtIndex:(int)i;
- (NSMutableArray*)	getMediaArray;
- (int)				numMediaItems;
- (void)			addMediaItem:(NSDictionary*)item;
@end
