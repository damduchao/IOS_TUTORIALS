//Library
#include "Game/Structs.h"
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
#import "Game/GameFunctions.hpp"

//UI
#import "UI/UIManager.h"
#import "UI/TypeSelectionView.h"
#include "Game/code.h"

@interface UIManager (Handlers)
- (void)handleZoomSwitch:(UISwitch *)sender;
- (void)handleZoomSlider:(UISlider *)sender;
- (void)onTimer;
@end

@implementation UIManager (Handlers)
- (void)handleZoomSwitch:(UISwitch *)sender {
    Vars.CameraZoom = sender.isOn;
}
- (void)handleZoomSlider:(UISlider *)sender {
    Vars.CameraSize = sender.value;
}
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
    
    [menu setMenuTitle:@"LAST OF EARTH"];
    [menu setMenuSubtitle:@"Premium Mod Menu v2.5"];

    // TAB 0: Giao chiến (Combat)
    [menu setTabIndex:0];
    [menu addSectionTitle:@"CHẾ ĐỘ CHIẾN ĐẤU"];
    [menu addFeatureSwitch:@"Bất tử (God Mode)" description:@"Nhân vật sẽ không bị mất máu."];
    [menu addFeatureSwitch:@"Vô hạn đạn" description:@"Bắn không giới hạn đạn."];
    [menu addFeatureSwitch:@"Không giật" description:@"Loại bỏ độ giật của súng."];
    [menu addSlider:@"Tốc độ bắn" max:10.0 min:1.0 value:1.0];
    
    // TAB 1: Hình ảnh (Visuals)
    [menu setTabIndex:1];
    [menu addSectionTitle:@"HIỂN THỊ CHI TIẾT"];
    [menu addFeatureSwitch:@"ESP Khung (Box)" description:@"Hiển thị khung bao quanh kẻ địch."];
    
    [menu addSectionTitle:@"CÀI ĐẶT CAMERA"];
    [menu addFeatureSwitch:@"Bật Zoom Camera" description:@"Thay đổi độ xa gần của tầm nhìn."];
    [[menu.switches objectForKey:@"Bật Zoom Camera"] addTarget:_UIManager action:@selector(handleZoomSwitch:) forControlEvents:UIControlEventValueChanged];
    
    [menu addSlider:@"Độ phóng (Zoom)" max:20.0 min:3.0 value:Vars.CameraSize];
    [[menu.sliders objectForKey:@"Độ phóng (Zoom)"] addTarget:_UIManager action:@selector(handleZoomSlider:) forControlEvents:UIControlEventValueChanged];

    [menu addComboSelector:@"Màu sắc ESP" options:@[@"Xanh lá", @"Đỏ", @"Vàng", @"Trắng"] selectedIndex:0 handler:^(NSInteger index) {
        // Logic đổi màu ở đây
        os_log(OS_LOG_DEFAULT, "Selected color index: %ld", (long)index);
    }];

    // TAB 2: Khác (Others)
    [menu setTabIndex:2];
    [menu addSectionTitle:@"TÍNH NĂNG KHÁC"];
    [menu addSlider:@"Tốc độ chạy" max:5.0 min:1.0 value:1.0];
    [menu addSlider:@"Độ cao nhảy" max:100.0 min:10.0 value:10.0];
    [menu addTextField:@"Dịch chuyển (XYZ)" placeholder:@"Nhập tọa độ: 100;20;100" handler:^(NSString *text) {
    }];
    [menu addButton:@"Hồi máu nhanh" withHandler:^{
    }];

    // TAB 3: Cài đặt (Settings)
    [menu setTabIndex:3];
    [menu addSectionTitle:@"TÙY CHỈNH GIAO DIỆN"];
    [menu addThemeSlider:@"Độ trong suốt" property:@"opacity" max:1.0 min:0.2 value:1.0];
    [menu addThemeSlider:@"Độ bo góc" property:@"corner" max:30.0 min:0.0 value:24.0];
    [menu addThemeSlider:@"Độ dày viền" property:@"border" max:5.0 min:0.0 value:1.0];
    
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
