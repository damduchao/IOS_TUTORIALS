#import <UIKit/UIKit.h>

@interface FloatingButton : UIButton

@property (nonatomic, assign) CGPoint lastLocation;
@property (nonatomic, copy) void (^onTap)(void);

+ (instancetype)buttonWithFrame:(CGRect)frame;
- (void)makeDraggable;

@end

@implementation FloatingButton

+ (instancetype)buttonWithFrame:(CGRect)frame {
    FloatingButton *button = [FloatingButton buttonWithType:UIButtonTypeCustom];
    button.frame = frame;
    [button setup];
    return button;
}

- (void)setup {
    self.backgroundColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:0.9];
    self.layer.cornerRadius = 15; // More square-ish rounded
    self.layer.shadowColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:0.6].CGColor;
    self.layer.shadowOffset = CGSizeMake(0, 4);
    self.layer.shadowOpacity = 0.8;
    self.layer.shadowRadius = 10;
    self.layer.borderWidth = 1.0;
    self.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.2].CGColor;
    
    [self setTitle:@"MENU" forState:UIControlStateNormal];
    [self setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    self.titleLabel.font = [UIFont systemFontOfSize:10 weight:UIFontWeightBold];
    
    [self addTarget:self action:@selector(buttonTapped) forControlEvents:UIControlEventTouchUpInside];
    [self makeDraggable];
}

- (void)buttonTapped {
    // Create visual feedback
    [UIView animateWithDuration:0.1 animations:^{
        self.backgroundColor = [UIColor colorWithRed:0.0 green:0.5 blue:0.9 alpha:1.0]; // Lighter blue
        self.transform = CGAffineTransformMakeScale(0.9, 0.9); // Scale down effect
    } completion:^(BOOL finished) {
        [UIView animateWithDuration:0.1 animations:^{
            self.backgroundColor = [UIColor systemBlueColor]; // Restore original color
            self.transform = CGAffineTransformIdentity; // Restore original size
        } completion:^(BOOL finished) {
            // Call the tap handler after the animation completes
            if (self.onTap) self.onTap();
        }];
    }];
}

- (void)makeDraggable {
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
    [self addGestureRecognizer:panGesture];
}

- (void)handlePan:(UIPanGestureRecognizer *)gesture {
    CGPoint translation = [gesture translationInView:self.superview];
    
    if (gesture.state == UIGestureRecognizerStateBegan) {
        self.lastLocation = self.center;
    }
    
    CGPoint newCenter = CGPointMake(self.lastLocation.x + translation.x, 
                                  self.lastLocation.y + translation.y);
    
    // Keep button within screen bounds
    CGRect screen = [UIScreen mainScreen].bounds;
    CGFloat halfWidth = self.frame.size.width / 2;
    CGFloat halfHeight = self.frame.size.height / 2;
    
    newCenter.x = MAX(halfWidth, MIN(screen.size.width - halfWidth, newCenter.x));
    newCenter.y = MAX(halfHeight, MIN(screen.size.height - halfHeight, newCenter.y));
    
    self.center = newCenter;
}

@end 