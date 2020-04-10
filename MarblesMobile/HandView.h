//
//  HandView.h
//  Marbles
//
//  Created by toddha on 2/4/13.
//  Copyright (c) 2013 toadgee.com. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CardView.h"
#import "Card.h"
#import "CardList.h"

@class TGMHandView;
@protocol TGMHandViewDelegate < NSObject >
-(void)selectedCardChanged:(TGMHandView *)sender;
@end

@interface TGMHandView : UIView
@property (readwrite, strong, nonatomic) TGMCardView *selectedCardView;
@property (readwrite, weak, nonatomic) id<TGMHandViewDelegate> delegate;
@property (readwrite, assign, nonatomic) TGMCardList *hand; // TODO : exposed?

@end

