//
//  UnlogoMediaItem.h
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/5/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface UnlogoMediaItem : NSObject <NSCoding> {
	NSDictionary* itemDict;
}

- setTitle:(NSString*) title;

@end
