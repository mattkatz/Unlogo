//
//  UnlogoMediaItem.m
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/5/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import "UnlogoMediaItem.h"


@implementation UnlogoMediaItem


// Encode an object for an archive
- (void)encodeWithCoder:(NSCoder *)coder
{
    [super encodeWithCoder:coder];
    //[coder encodeObject:name forKey:@"Name"];
    //[coder encodeInteger:age forKey:@"Age"];
}
// Decode an object from an archive
- (id)initWithCoder:(NSCoder *)coder
{
    self = [super initWithCoder:coder];
    //name = [[coder decodeObjectForKey:@"Name"] retain];
    //age  = [coder decodeIntegerForKey:@"Age"];
}


- setTitle:(NSString*) title
{
	[itemDict setValue:title forKey:@"title"];
}

@end
