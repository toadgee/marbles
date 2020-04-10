//
//  UIImageExtensions.m
//  Marbles
//
//  Created by Todd Harris on 12/16/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import "UIImageExtensions.h"

@implementation UIImage (MarblesExtensions)

- (UIImage *)copy
{
    CGImageRef newCgIm = CGImageCreateCopy(self.CGImage);
    UIImage* retval = [[UIImage alloc] initWithCGImage:newCgIm scale:self.scale orientation:self.imageOrientation];
	CGImageRelease(newCgIm);
	return retval;
}

- (UIImage *)subimageWithRect:(CGRect)rect
{
    CGImageRef cgImg = CGImageCreateWithImageInRect(self.CGImage, rect);
    UIImage* retval = [UIImage imageWithCGImage:cgImg];
	CGImageRelease(cgImg);
	return retval;
}

- (UIImage *)imageRotatedByAngle:(CGFloat)angle
{
    CGFloat angleInRadians = (CGFloat)(angle * (M_PI / 180));
    CGFloat width = self.size.width;
    CGFloat height = self.size.height;

    CGRect imgRect = CGRectMake(0, 0, width, height);
    CGAffineTransform transform = CGAffineTransformMakeRotation(angleInRadians);
    CGRect rotatedRect = CGRectApplyAffineTransform(imgRect, transform);

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef bmContext = CGBitmapContextCreate(NULL,
                                                   (size_t)(rotatedRect.size.width),
                                                   (size_t)(rotatedRect.size.height),
                                                   8,
                                                   0,
                                                   colorSpace,
                                                   (CGBitmapInfo)kCGImageAlphaPremultipliedFirst);
    CGContextSetAllowsAntialiasing(bmContext, YES);
    CGContextSetShouldAntialias(bmContext, YES);
    CGContextSetInterpolationQuality(bmContext, kCGInterpolationHigh);
    CGColorSpaceRelease(colorSpace);
    CGContextTranslateCTM(bmContext,
                          +(rotatedRect.size.width/2),
                          +(rotatedRect.size.height/2));
    CGContextRotateCTM(bmContext, angleInRadians);
    CGContextTranslateCTM(bmContext,
                          -(rotatedRect.size.width/2),
                          -(rotatedRect.size.height/2));
    CGContextDrawImage(bmContext, CGRectMake(0, 0,
                                             rotatedRect.size.width,
                                             rotatedRect.size.height),
                       self.CGImage);




    CGImageRef rotatedImage = CGBitmapContextCreateImage(bmContext);
    CFRelease(bmContext);

    UIImage* retval = [[UIImage alloc] initWithCGImage:rotatedImage scale:self.scale orientation:self.imageOrientation];
	CGImageRelease(rotatedImage);
	return retval;
}

@end
