//
//  ImageZoomController.h
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/3/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface ImageZoomController : UIViewController <UIScrollViewDelegate> {
	IBOutlet UIScrollView *myScrollView;
	UIImageView *myImage;
}

@property (nonatomic, retain) UIScrollView *myScrollView;
@property (nonatomic, retain) UIImageView *myImage;


- (void) setPhoto:(NSString*)path;

@end
