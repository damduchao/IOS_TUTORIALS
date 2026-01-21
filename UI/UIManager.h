#import <UIKit/UIKit.h>
#import "FloatingButton.h"
#import "MenuView.h"

@interface UIManager : NSObject

@property (nonatomic, strong) FloatingButton *floatingButton;
@property (nonatomic, strong) MenuView *menu;

+ (instancetype)shared;
- (void)setupUI;
- (void)toggleMenu;

@end

@implementation UIManager {
    BOOL _menuVisible;
}

+ (instancetype)shared {
    static UIManager *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[UIManager alloc] init];
    });
    return sharedInstance;
}

- (void)setupUI {
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    if (!window) return;

    // Adaptive Sizing
    CGSize screenSize = window.bounds.size;
    BOOL isLandscape = screenSize.width > screenSize.height;
    
    // Calculate compact size
    CGFloat menuWidth = isLandscape ? 380 : 320;
    CGFloat menuHeight = isLandscape ? 340 : 450;
    
    CGRect menuFrame = CGRectMake((screenSize.width - menuWidth) / 2,
                                 (screenSize.height - menuHeight) / 2,
                                 menuWidth,
                                 menuHeight);
    
    // Setup floating button
    CGFloat buttonSize = 50;
    if (!self.floatingButton) {
        CGRect buttonFrame = CGRectMake(screenSize.width - buttonSize - 20, screenSize.height / 2, buttonSize, buttonSize);
        self.floatingButton = [FloatingButton buttonWithFrame:buttonFrame];
        __block UIManager *blockSelf = self;
        self.floatingButton.onTap = ^{ [blockSelf toggleMenu]; };
    }
    [window addSubview:self.floatingButton];
    [window bringSubviewToFront:self.floatingButton];
    
    // Create or update menu
    if (!self.menu) {
        self.menu = [MenuView menuWithFrame:menuFrame];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onOrientationChange) name:UIDeviceOrientationDidChangeNotification object:nil];
    } else {
        [UIView animateWithDuration:0.3 animations:^{
            self.menu.frame = menuFrame;
            [self.menu updateLayout];
        }];
    }
}

- (void)onOrientationChange {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self setupUI];
    });
}

- (void)toggleMenu {
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    if (!window) return;

    if (self.menu.superview) {
        [self.menu removeFromSuperview];
    } else {
        self.menu.alpha = 1.0;
        self.menu.transform = CGAffineTransformIdentity;
        [window addSubview:self.menu];
        [window bringSubviewToFront:self.menu];
    }
}

@end 