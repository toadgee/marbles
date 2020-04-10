//
//  UIImageExtensions.h
//  Marbles
//
//  Created by Todd Harris on 12/16/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIImage (MarblesExtensions)

- (UIImage *)copy;
- (UIImage *)subimageWithRect:(CGRect)rect;
- (UIImage *)imageRotatedByAngle:(CGFloat)angle;

@end
