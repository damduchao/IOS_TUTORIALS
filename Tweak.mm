//Library
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import <AudioToolbox/AudioToolbox.h>
#import <os/log.h>
#import "pthread.h"
#include <math.h>
#include <deque>
#include <vector>
#include <fstream>

//Config
#import "Config.h"
#include "Lib/Obfuscate.h"
#include "Lib/Vector3.h"
#include "Lib/Vector2.h"
#include "Lib/Quaternion.h"
#include "Lib/Monostring.h"
//Game
#import "Lib/Resolver.hpp"

//UI
#import "UI/UIManager.h"
#import "UI/TypeSelectionView.h"
#include "Game/GameFunction.h"

@interface UIManager (HackLoop)
- (void)onTimer;
@end

@implementation UIManager (HackLoop)
- (void)onTimer {
    render_lop();
}
@end

static UIManager *_UIManager = [UIManager shared];

static void SetupUI() {
    [_UIManager setupUI];

    MenuView *menu = _UIManager.menu;
    menu.telegramURL = @"https://t.me/ioshacktutorial";
    menu.discordURL = @"https://discord.gg/iostutorials";
    
    [menu setMenuTitle:@"GGD PREMIUM CHEATS"];
    [menu setMenuSubtitle:@"Menu By Hào Đàm v2.5"];

    // TAB 0: Giao chiến (Combat)
    [menu setTabIndex:0];
    [menu addSectionTitle:@"CHẾ ĐỘ CHIẾN ĐẤU"];
    
    [menu addFeatureSwitch:@"Tăng tốc (Speedhack)" description:@"Di chuyển nhanh hơn bình thường." handler:^(BOOL isOn) {
        Vars.Speedhack = isOn;
    }];
    
    [menu addSlider:@"Tốc độMultiplier" max:10.0 min:1.0 value:Vars.SpeedMultiplier handler:^(CGFloat value) {
        Vars.SpeedMultiplier = value;
    }];

    [menu addFeatureSwitch:@"Hồi chiêu giết" description:@"Xóa thời gian chờ hồi chiêu giết." handler:^(BOOL isOn) {
        Vars.NoKillCooldown = isOn;
    }];
    
    [menu addFeatureSwitch:@"Spam Phá hoại" description:@"Xóa hồi chiêu Sabotage." handler:^(BOOL isOn) {
        Vars.SabotageSpam = isOn;
    }];
    
    // TAB 1: Hình ảnh (Visuals)
    [menu setTabIndex:1];
    [menu addSectionTitle:@"CAMERA VISUALS"];
    
    [menu addFeatureSwitch:@"Xóa sương mù" description:@"Hiển thị toàn bộ bản đồ rõ nét." handler:^(BOOL isOn) {
        Vars.NoFog = isOn;
    }];
    
    [menu addSectionTitle:@"CAMERA SETTINGS"];
    [menu addFeatureSwitch:@"Bật Zoom Camera" description:@"Thay đổi độ xa gần của tầm nhìn." handler:^(BOOL isOn) {
        Vars.CameraZoom = isOn;
    }];
    
    [menu addSlider:@"Độ phóng (Zoom)" max:20.0 min:3.0 value:Vars.CameraSize handler:^(CGFloat value) {
        Vars.CameraSize = value;
    }];

    // TAB 2: Khác (Others)
    [menu setTabIndex:2];
    [menu addSectionTitle:@"OTHER FEATURES"];
    
    [menu addFeatureSwitch:@"Đi xuyên tường" description:@"Có thể đi qua mọi chướng ngại vật." handler:^(BOOL isOn) {
        Vars.Wallhack = isOn;
    }];
    
    [menu addButton:@"Hoàn thành Nhiệm vụ" withHandler:^{
        Vars.InstantTask = true;
    }];

    // TAB 3: Cài đặt (Settings)
    [menu setTabIndex:3];
    [menu addSectionTitle:@"TÙY CHỈNH GIAO DIỆN"];
    
    [menu addComboSelector:@"Màu chủ đạo" options:@[@"Xanh dương", @"Đỏ rực", @"Xanh lá", @"Vàng", @"Tím", @"Hồng"] selectedIndex:0 handler:^(NSInteger index) {
        UIColor *selectedColor;
        switch (index) {
            case 0: selectedColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:1.0]; break;
            case 1: selectedColor = [UIColor colorWithRed:255.0/255.0 green:59.0/255.0 blue:48.0/255.0 alpha:1.0]; break;
            case 2: selectedColor = [UIColor colorWithRed:52.0/255.0 green:199.0/255.0 blue:89.0/255.0 alpha:1.0]; break;
            case 3: selectedColor = [UIColor colorWithRed:255.0/255.0 green:204.0/255.0 blue:0.0/255.0 alpha:1.0]; break;
            case 4: selectedColor = [UIColor colorWithRed:175.0/255.0 green:82.0/255.0 blue:222.0/255.0 alpha:1.0]; break;
            case 5: selectedColor = [UIColor colorWithRed:255.0/255.0 green:45.0/255.0 blue:85.0/255.0 alpha:1.0]; break;
            default: selectedColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:1.0]; break;
        }
        [menu setMenuAccentColor:selectedColor];
    }];

    [menu addThemeSlider:@"Độ trong suốt" property:@"opacity" max:1.0 min:0.2 value:1.0 handler:nil];
    [menu addThemeSlider:@"Độ bo góc" property:@"corner" max:30.0 min:0.0 value:24.0 handler:nil];
    [menu addThemeSlider:@"Độ dày viền" property:@"border" max:5.0 min:0.0 value:1.0 handler:nil];
    
    [menu addSectionTitle:@"HÀNH ĐỘNG"];
    [menu addButton:@"Reset Cài đặt" withHandler:^{
        [menu setMenuCornerRadius:24.0];
        [menu setMenuBorderWidth:1.0];
        menu.alpha = 1.0;
    }];
    
    [menu updateLayout];
}

static void initializeIl2Cpp(void) {
    NSString *appPath = [[NSBundle mainBundle] bundlePath];
    NSString *mainBundleName = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleExecutable"];
    NSString *unityFrameworkPath = [appPath stringByAppendingPathComponent:@"Frameworks/UnityFramework.framework/UnityFramework"];
    void *handle = dlopen([unityFrameworkPath fileSystemRepresentation], RTLD_LAZY);
    if (!handle) {
        unityFrameworkPath = [appPath stringByAppendingPathComponent:mainBundleName];
        handle = dlopen([unityFrameworkPath fileSystemRepresentation], RTLD_LAZY);
    }
    if (handle) Resolve::Init(handle, Resolve::Mode::Il2Cpp);
}

static void Initialize() {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(WAIT_TIME * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        initializeIl2Cpp();
        game_sdk->init();
        CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:_UIManager selector:@selector(onTimer)];
        [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
        SetupUI(); 
    });
}

static void didFinishLaunching(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef info) {
    Initialize();
}

static void launchEvent(void) {
    CFNotificationCenterAddObserver(CFNotificationCenterGetLocalCenter(), NULL, &didFinishLaunching, (CFStringRef)UIApplicationDidFinishLaunchingNotification, NULL, CFNotificationSuspensionBehaviorDrop);
}

__attribute__((constructor)) static void initialize(void) { 
    launchEvent();
}
