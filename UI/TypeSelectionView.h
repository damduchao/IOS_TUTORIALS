#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, SelectionType) {
    SelectionTypeResource,
    SelectionTypeTool,
    SelectionTypeEquipment
};

typedef void (^TypeSelectionCompletionHandler)(NSInteger selectedIndex);

@interface TypeSelectionView : UIView {
    CGPoint _lastTouchPoint;
    UIView *_dragHandle;
}

@property (nonatomic, copy) TypeSelectionCompletionHandler completionHandler;

+ (instancetype)showWithType:(SelectionType)type fromView:(UIView *)sourceView completion:(TypeSelectionCompletionHandler)completion;
- (void)dismiss;

@end

@implementation TypeSelectionView

+ (instancetype)showWithType:(SelectionType)type fromView:(UIView *)sourceView completion:(TypeSelectionCompletionHandler)completion {
    TypeSelectionView *view = [[TypeSelectionView alloc] initWithType:type sourceView:sourceView];
    view.completionHandler = completion;
    
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    view.frame = window.bounds;
    [window addSubview:view];
    
    [view animateIn];
    return view;
}

- (instancetype)initWithType:(SelectionType)type sourceView:(UIView *)sourceView {
    if (self = [super init]) {
        self.backgroundColor = [UIColor colorWithWhite:0 alpha:0.5];
        
        // Create container view
        UIView *container = [[UIView alloc] init];
        container.backgroundColor = [UIColor colorWithWhite:0.1 alpha:0.95];
        container.layer.cornerRadius = 15;
        container.clipsToBounds = YES;
        [self addSubview:container];
        
        // Add drag handle
        _dragHandle = [[UIView alloc] init];
        _dragHandle.backgroundColor = [UIColor colorWithWhite:0.3 alpha:1.0];
        _dragHandle.layer.cornerRadius = 2.5;
        [container addSubview:_dragHandle];
        
        // Title
        UILabel *titleLabel = [[UILabel alloc] init];
        titleLabel.textColor = [UIColor whiteColor];
        titleLabel.font = [UIFont boldSystemFontOfSize:18];
        titleLabel.textAlignment = NSTextAlignmentCenter;
        
        // Setup options based on type
        NSArray *options;
        switch (type) {
            case SelectionTypeResource:
                titleLabel.text = @"Select Resource";
                options = @[
                    @"None",
                    @"Gold",
                    @"Gem",
                    @"Wood",
                    @"Stone",
                    @"Potato",
                    @"Apple",
                    @"Iron Ore",
                    @"Plank",
                    @"Brick",
                    @"Iron Ingot",
                    @"Range Weapon Slab",
                    @"Tool Slab",
                    @"Melee Weapon Slab",
                    @"Hat Slab",
                    @"Wear Slab",
                    @"Boots Slab",
                    @"Accessories Slab"
                ];
                break;
            case SelectionTypeTool:
                titleLabel.text = @"Select Tool";
                options = @[
                    @"Axe",
                    @"Pickaxe",
                    @"Shovel",
                    @"Sickle",
                    @"Fist",
                    @"Bow"
                ];
                break;
            case SelectionTypeEquipment:
                titleLabel.text = @"Select Equipment";
                options = @[
                    @"Bat",
                    @"Nailed Bat",
                    @"Iron Pipe",
                    @"Fire Axe",
                    @"Katana",
                    @"Shotgun",
                    @"Bow",
                    @"Baseball Cap",
                    @"Pot",
                    @"Goggles",
                    @"Gas Mask",
                    @"Rugby Helmet",
                    @"Gas Monitor",
                    @"Leather Bag",
                    @"Green Bag",
                    @"Military Bag",
                    @"Teddy Bear Bag",
                    @"Luxury Bag"
                ];
                break;
        }
        
        // Layout
        CGFloat containerWidth = 280;
        CGFloat buttonHeight = 45; 
        CGFloat headerHeight = 60; 
        CGFloat visibleButtons = 6; // Number of visible buttons
        CGFloat scrollViewHeight = buttonHeight * visibleButtons;
        CGFloat containerHeight = headerHeight + scrollViewHeight + 10; 
        
        // Position container 
        CGFloat screenWidth = [UIScreen mainScreen].bounds.size.width;
        CGFloat screenHeight = [UIScreen mainScreen].bounds.size.height;
        CGFloat xPos = MIN(screenWidth - containerWidth - 20, screenWidth * 0.7);
        CGFloat yPos = (screenHeight - containerHeight) / 2;
        
        container.frame = CGRectMake(xPos, yPos, containerWidth, containerHeight);
        
        // Position drag handle
        _dragHandle.frame = CGRectMake(20, 10, containerWidth - 40, 4);
        
        // Position title below drag handle
        titleLabel.frame = CGRectMake(0, 20, containerWidth, 30);
        [container addSubview:titleLabel];
        
        // Create scroll view for buttons
        UIScrollView *scrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(0, headerHeight, containerWidth, scrollViewHeight)];
        scrollView.contentSize = CGSizeMake(containerWidth, (buttonHeight * options.count));
        scrollView.showsVerticalScrollIndicator = YES;
        scrollView.indicatorStyle = UIScrollViewIndicatorStyleWhite;
        [container addSubview:scrollView];
        
        // Add buttons to scroll view
        for (NSInteger i = 0; i < options.count; i++) {
            UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
            button.frame = CGRectMake(20, (i * buttonHeight), containerWidth - 40, buttonHeight - 8);
            [button setTitle:options[i] forState:UIControlStateNormal];
            [button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
            button.backgroundColor = [UIColor systemBlueColor];
            button.layer.cornerRadius = 8;
            button.tag = i;
            [button addTarget:self action:@selector(optionSelected:) forControlEvents:UIControlEventTouchUpInside];
            
            // Add hover effect
            [button addTarget:self action:@selector(buttonTouchDown:) forControlEvents:UIControlEventTouchDown];
            [button addTarget:self action:@selector(buttonTouchUp:) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
            
            [scrollView addSubview:button];
        }
        
        // Add pan gesture recognizer for dragging
        UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
        [container addGestureRecognizer:panGesture];
        
        // Add tap gesture to dismiss when tapping outside
        UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(backgroundTapped:)];
        [self addGestureRecognizer:tapGesture];
    }
    return self;
}

- (void)handlePan:(UIPanGestureRecognizer *)gesture {
    UIView *container = gesture.view;
    CGPoint translation = [gesture translationInView:self];
    
    if (gesture.state == UIGestureRecognizerStateBegan) {
        _lastTouchPoint = container.center;
    }
    
    CGPoint newCenter = CGPointMake(_lastTouchPoint.x + translation.x, _lastTouchPoint.y + translation.y);
    
    // Keep the view within screen bounds
    CGFloat halfWidth = container.bounds.size.width / 2;
    CGFloat halfHeight = container.bounds.size.height / 2;
    CGRect screen = [UIScreen mainScreen].bounds;
    
    newCenter.x = MAX(halfWidth, MIN(screen.size.width - halfWidth, newCenter.x));
    newCenter.y = MAX(halfHeight, MIN(screen.size.height - halfHeight, newCenter.y));
    
    container.center = newCenter;
}

- (void)buttonTouchDown:(UIButton *)sender {
    [UIView animateWithDuration:0.1 animations:^{
        sender.transform = CGAffineTransformMakeScale(0.95, 0.95);
        sender.backgroundColor = [UIColor colorWithRed:0.2 green:0.5 blue:0.9 alpha:1.0];
    }];
}

- (void)buttonTouchUp:(UIButton *)sender {
    [UIView animateWithDuration:0.1 animations:^{
        sender.transform = CGAffineTransformIdentity;
        sender.backgroundColor = [UIColor systemBlueColor];
    }];
}

- (void)optionSelected:(UIButton *)sender {
    if (self.completionHandler) {
        self.completionHandler(sender.tag);
    }
    [self dismiss];
}

- (void)backgroundTapped:(UITapGestureRecognizer *)gesture {
    if (gesture.view == self) {
        [self dismiss];
    }
}

- (void)animateIn {
    self.alpha = 0;
    UIView *container = self.subviews.firstObject;
    container.transform = CGAffineTransformMakeScale(0.8, 0.8);
    
    [UIView animateWithDuration:0.3 animations:^{
        self.alpha = 1;
        container.transform = CGAffineTransformIdentity;
    }];
}

- (void)dismiss {
    [UIView animateWithDuration:0.2 animations:^{
        self.alpha = 0;
        UIView *container = self.subviews.firstObject;
        container.transform = CGAffineTransformMakeScale(0.8, 0.8);
    } completion:^(BOOL finished) {
        [self removeFromSuperview];
    }];
}

@end 