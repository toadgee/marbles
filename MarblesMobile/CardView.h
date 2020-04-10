//
//  CardView.h
//  Marbles
//
//  Created by toddha on 12/17/12.
//  Copyright (c) 2012 toadgee.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Card.h"

@class TGMCardView;
@protocol TGMCardViewDelegate < NSObject >
-(void)selectedCard:(TGMCardView *)sender;
@end


@interface TGMCardView : UIImageView

@property (readwrite, assign, nonatomic) TGMCard *card;
@property (readwrite, weak, nonatomic) IBOutlet id<TGMCardViewDelegate> delegate;
@property (readwrite, assign, nonatomic, getter=isSelected) BOOL selected;
@property (readwrite, copy, nonatomic) NSString *name;

+(CGSize)cardImageSize;

@end
