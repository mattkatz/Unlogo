//
//  Preferences.m
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/4/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import "Preferences.h"


@implementation Preferences
@synthesize prefsDict;
@synthesize prefsFilePath;

static Preferences *sharedInstance = nil;


+ (Preferences*)sharedInstance
{
    if (sharedInstance == nil)
	{
        sharedInstance = [[self alloc] init];
    }
    return sharedInstance;
}


-(id)init
{
    if (self = [super init])
	{
		[self loadPrefs];// Load or init the preferences
    }
    return self;
}


- (void)dealloc
{
    // Ensure the prefs are saved
    [self savePrefs];
    // release resources
    [prefsDict release];
    [prefsFilePath release];
    [super dealloc];
}

// Load the prefs from file, if the file does not exist it is created
// and some defaults set
- (void)loadPrefs
{
    // If the preferences file path is not yet set, ensure it is initialised
    if (prefsFilePath == nil) {
        [self initPrefsFilePath];
    }
	
    // If the preferences file exists, then load it
	
    if ([[NSFileManager defaultManager] fileExistsAtPath:prefsFilePath]) {
        self.prefsDict = [[NSMutableDictionary alloc] initWithContentsOfFile:prefsFilePath];
    } else {
        // Initialise a new dictionary
        self.prefsDict = [[NSMutableDictionary alloc] init];
    }
}

- (void)initPrefsFilePath
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	self.prefsFilePath = [documentsDirectory stringByAppendingPathComponent:@"unlogo_prefs.xml"];
	[prefsFilePath retain];
}


- (void)savePrefs
{
	NSLog(@"Saving to %@: %@", prefsFilePath, prefsDict);
    [prefsDict writeToFile:prefsFilePath atomically:YES];
}


#pragma mark -
#pragma mark Unlogo-Specific Stuff

- (NSMutableArray*)getMediaArray
{
	NSMutableArray* media = [prefsDict objectForKey:@"media"];
	if(media==nil)
	{
		media = [NSMutableArray array];
		[prefsDict setObject:media forKey:@"media"];
	}
	return media;
}

-(void)addMediaItem:(NSDictionary*)item
{
	[[self getMediaArray] addObject:item];
	[self savePrefs];
}

-(int)numMediaItems 
{
	return [[self getMediaArray] count];
}

- (void)deleteMediaItemAtIndex:(int)i
{
	NSMutableArray* media = [self getMediaArray];
	if(i<[media count])
	{
		NSFileManager *fileManager = [[NSFileManager alloc] init];
		NSDictionary* item = [media objectAtIndex:i];
		
		
		NSArray* files = [NSArray arrayWithObjects:@"original", @"thumbnail", @"unlogo", nil];
		for(int i=0; i<[files count]; i++)
		{
			NSString* path = [item objectForKey: [files objectAtIndex:i]];
			if([fileManager fileExistsAtPath:path])
			{
				NSError* err;
				[fileManager removeItemAtPath:path error:&err];
				if(err == NULL)
				{
					NSLog(@"Deleted %@", path);
				}
				else
				{
					NSLog(@"WARNING:  Couldn't delete %@", path);
					//NSLog(@"Err desc-%@", [err localizedDescription]);
					//NSLog(@"Err reason-%@", [err localizedFailureReason]);
				}

			}
		}
		[media removeObjectAtIndex:i];
	}
}

-(NSDictionary*)getMediaItemAtIndex:(int)i
{
	return [[self getMediaArray] objectAtIndex:i];
}

- (NSDictionary*)getMediaItemWithID:(NSString*)media_id
{
	NSArray* mediaItems = [self getMediaArray];
	for(int i=0; i<[mediaItems count]; i++)
	{
		NSString* this_id = [[mediaItems objectAtIndex:i] objectForKey:@"media_id"];
		if([this_id isEqualToString:media_id])
		{
			return [mediaItems objectAtIndex:i];
		}
	}
	return nil;
}

@end
