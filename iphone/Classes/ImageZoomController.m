    //
//  ImageZoomController.m
//  Unlogo
//
//  Created by Jeffrey Crouse on 8/3/10.
//  Copyright 2010 Eyebeam. All rights reserved.
//

#import "ImageZoomController.h"


@implementation ImageZoomController

@synthesize myScrollView, myImage;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

- (void) setPhoto:(NSString*)path
{
	UIImageView *tempImageView = [[UIImageView alloc] initWithImage:[UIImage imageWithContentsOfFile:path]];
	[self setMyImage:tempImageView];
	[tempImageView release];
	
	myScrollView.contentSize = CGSizeMake(myImage.frame.size.width, myImage.frame.size.height);
	myScrollView.maximumZoomScale = 4.0;
	myScrollView.minimumZoomScale = 0.2;

	myScrollView.clipsToBounds = YES;
	myScrollView.delegate = self;
    [myScrollView addSubview:myImage];	
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView {
	return myImage;
}


/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[myScrollView release];
	[myImage release];
    [super dealloc];
}


@end
