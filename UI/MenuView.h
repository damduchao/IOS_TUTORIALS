@interface MenuView : UIView <UITextFieldDelegate>

@property (nonatomic, assign) CGPoint lastLocation;
@property (nonatomic, strong) NSMutableDictionary *switches;
@property (nonatomic, strong) NSMutableDictionary *sliders;
@property (nonatomic, strong) NSMutableDictionary *sliderLabels;
@property (nonatomic, strong) NSMutableDictionary *buttons;
@property (nonatomic, strong) NSMutableDictionary *textFields;
@property (nonatomic, strong) UIColor *accentColor;
@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) UILabel *subtitleLabel;
@property (nonatomic, strong) UIScrollView *scrollView;
@property (nonatomic, strong) UIView *contentView;
@property (nonatomic, strong) UIVisualEffectView *blurEffectView;
@property (nonatomic, strong) UISegmentedControl *tabBar;
@property (nonatomic, assign) NSInteger currentCategoryCounter;

+ (instancetype)menuWithFrame:(CGRect)frame;
- (void)makeDraggable;
- (void)addFeatureSwitch:(NSString *)title description:(NSString *)desc handler:(void (^)(BOOL isOn))handler;
- (void)addSlider:(NSString *)title max:(CGFloat)max min:(CGFloat)min value:(CGFloat)value handler:(void (^)(CGFloat value))handler;
- (void)addButton:(NSString *)title withHandler:(void (^)(void))handler;
- (void)addComboSelector:(NSString *)title options:(NSArray *)options selectedIndex:(NSInteger)index handler:(void (^)(NSInteger selectedIndex))handler;
- (void)addTextField:(NSString *)title placeholder:(NSString *)placeholder handler:(void (^)(NSString *text))handler;
@property (nonatomic, strong) NSString *telegramURL;
@property (nonatomic, strong) NSString *discordURL;

- (void)addSectionTitle:(NSString *)title;
- (void)setTabIndex:(NSInteger)index;
- (void)addThemeSlider:(NSString *)title property:(NSString *)prop max:(CGFloat)max min:(CGFloat)min value:(CGFloat)value handler:(void (^)(CGFloat value))handler;
- (void)updateLayout;
- (void)setMenuAccentColor:(UIColor *)color;
- (void)setMenuGlassEffect:(BOOL)enabled;
- (void)setMenuCornerRadius:(CGFloat)radius;
- (void)setMenuBorderWidth:(CGFloat)width;
- (void)setMenuTitle:(NSString *)title;
- (void)setMenuSubtitle:(NSString *)subtitle;
- (void)setFooterText:(NSString *)text;
@end

@implementation MenuView

+ (instancetype)menuWithFrame:(CGRect)frame {
    MenuView *menu = [[MenuView alloc] initWithFrame:frame];
    [menu setup];
    return menu;
}

- (void)setup {
    self.accentColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:1.0];
    self.layer.cornerRadius = 24;
    self.layer.masksToBounds = YES;
    self.layer.borderWidth = 1.0;
    self.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.1].CGColor;
    self.currentCategoryCounter = 0;
    
    UIBlurEffect *blurEffect = [UIBlurEffect effectWithStyle:UIBlurEffectStyleDark];
    self.blurEffectView = [[UIVisualEffectView alloc] initWithEffect:blurEffect];
    self.blurEffectView.frame = self.bounds;
    self.blurEffectView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self addSubview:self.blurEffectView];
    
    UIView *gradientOverlay = [[UIView alloc] initWithFrame:self.bounds];
    gradientOverlay.backgroundColor = [UIColor colorWithRed:15.0/255.0 green:15.0/255.0 blue:25.0/255.0 alpha:0.4];
    [self addSubview:gradientOverlay];
    
    // Header
    self.titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 20, 200, 24)];
    self.titleLabel.text = @"BOORIRPG PREMIUM";
    self.titleLabel.textColor = [UIColor whiteColor];
    self.titleLabel.font = [UIFont systemFontOfSize:18 weight:UIFontWeightBold];
    [self addSubview:self.titleLabel];
    
    // Social Buttons in Header (Top Right)
    UIButton *tgBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    tgBtn.frame = CGRectMake(self.frame.size.width - 110, 20, 28, 28);
    tgBtn.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.1];
    tgBtn.layer.cornerRadius = 14;
    tgBtn.tag = 5001;
    [tgBtn setTitle:@"✈" forState:UIControlStateNormal];
    
    UIButton *dsBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    dsBtn.frame = CGRectMake(self.frame.size.width - 77, 20, 28, 28);
    dsBtn.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.1];
    dsBtn.layer.cornerRadius = 14;
    dsBtn.tag = 5002;
    [dsBtn setTitle:@"◈" forState:UIControlStateNormal];
    
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 13.0) {
        [tgBtn setImage:[UIImage systemImageNamed:@"paperplane.fill"] forState:UIControlStateNormal];
        tgBtn.tintColor = [UIColor whiteColor];
        tgBtn.imageEdgeInsets = UIEdgeInsetsMake(6, 6, 6, 6);
        [tgBtn setTitle:@"" forState:UIControlStateNormal];
        
        [dsBtn setImage:[UIImage systemImageNamed:@"bubble.left.and.bubble.right.fill"] forState:UIControlStateNormal];
        dsBtn.tintColor = [UIColor whiteColor];
        dsBtn.imageEdgeInsets = UIEdgeInsetsMake(6, 6, 6, 6);
        [dsBtn setTitle:@"" forState:UIControlStateNormal];
    }
    
    [tgBtn addTarget:self action:@selector(socialTapped:) forControlEvents:UIControlEventTouchUpInside];
    [dsBtn addTarget:self action:@selector(socialTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:tgBtn];
    [self addSubview:dsBtn];

    UIButton *closeButton = [UIButton buttonWithType:UIButtonTypeCustom];
    closeButton.frame = CGRectMake(self.frame.size.width - 40, 20, 28, 28);
    [closeButton setTitle:@"✕" forState:UIControlStateNormal];
    [closeButton setTitleColor:[UIColor colorWithRed:1.0 green:0.23 blue:0.19 alpha:1.0] forState:UIControlStateNormal];
    closeButton.backgroundColor = [UIColor colorWithRed:1.0 green:0.23 blue:0.19 alpha:0.1];
    closeButton.layer.cornerRadius = 8;
    [closeButton addTarget:self action:@selector(closeButtonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:closeButton];
    
    self.subtitleLabel = [[UILabel alloc] initWithFrame:CGRectMake(20, 44, self.frame.size.width - 40, 15)];
    self.subtitleLabel.text = @"ADVANCED SURVIVAL V2.5";
    self.subtitleLabel.textColor = [UIColor colorWithWhite:0.7 alpha:1.0];
    self.subtitleLabel.font = [UIFont systemFontOfSize:10 weight:UIFontWeightMedium];
    [self addSubview:self.subtitleLabel];
    
    // Tabs
    NSArray *items = @[@"Giao chiến", @"Hình ảnh", @"Khác", @"Cài đặt"];
    self.tabBar = [[UISegmentedControl alloc] initWithItems:items];
    self.tabBar.frame = CGRectMake(10, 75, self.frame.size.width - 20, 35);
    self.tabBar.selectedSegmentIndex = 0;
    
    // Fix text color for Normal and Selected states
    NSDictionary *normalAttributes = @{
        NSForegroundColorAttributeName: [UIColor colorWithWhite:1.0 alpha:0.5],
        NSFontAttributeName: [UIFont systemFontOfSize:12 weight:UIFontWeightMedium]
    };
    NSDictionary *selectedAttributes = @{
        NSForegroundColorAttributeName: [UIColor whiteColor],
        NSFontAttributeName: [UIFont systemFontOfSize:12 weight:UIFontWeightBold]
    };
    
    [self.tabBar setTitleTextAttributes:normalAttributes forState:UIControlStateNormal];
    [self.tabBar setTitleTextAttributes:selectedAttributes forState:UIControlStateSelected];
    
    // Set background color for the selected tab
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 13.0) {
        self.tabBar.selectedSegmentTintColor = self.accentColor;
        self.tabBar.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.1];
    }
    
    [self.tabBar addTarget:self action:@selector(tabChanged:) forControlEvents:UIControlEventValueChanged];
    [self addSubview:self.tabBar];
    
    self.scrollView = [[UIScrollView alloc] initWithFrame:CGRectMake(0, 115, self.frame.size.width, self.frame.size.height - 150)];
    self.scrollView.showsVerticalScrollIndicator = NO;
    [self addSubview:self.scrollView];
    
    self.contentView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.scrollView.frame.size.width, 0)];
    [self.scrollView addSubview:self.contentView];
    
    // Footer Credit
    UILabel *footer = [[UILabel alloc] initWithFrame:CGRectMake(0, self.frame.size.height - 30, self.frame.size.width, 25)];
    footer.textAlignment = NSTextAlignmentCenter;
    footer.textColor = [UIColor colorWithWhite:1.0 alpha:0.3];
    footer.font = [UIFont systemFontOfSize:10 weight:UIFontWeightBold];
    footer.text = @"CREDIT: IOS TUTORIALS";
    [self addSubview:footer];

    self.switches = [NSMutableDictionary dictionary];
    self.sliders = [NSMutableDictionary dictionary];
    self.sliderLabels = [NSMutableDictionary dictionary];
    self.buttons = [NSMutableDictionary dictionary];
    self.textFields = [NSMutableDictionary dictionary];
    [self makeDraggable];
}

- (void)socialTapped:(UIButton *)sender {
    NSString *urlStr = (sender.tag == 5001) ? self.telegramURL : self.discordURL;
    if (urlStr) {
        NSURL *url = [NSURL URLWithString:urlStr];
        if ([[UIApplication sharedApplication] canOpenURL:url]) {
            [[UIApplication sharedApplication] openURL:url options:@{} completionHandler:nil];
        }
    }
}

- (void)addThemeSlider:(NSString *)title property:(NSString *)prop max:(CGFloat)max min:(CGFloat)min value:(CGFloat)value handler:(void (^)(CGFloat value))handler {
    [self addSlider:title max:max min:min value:value handler:handler];
    UISlider *sl = self.sliders[title];
    objc_setAssociatedObject(sl, "themeProp", prop, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

- (void)setFooterText:(NSString *)text {
    for (UIView *v in self.subviews) {
        if ([v isKindOfClass:[UILabel class]] && v.frame.origin.y > self.frame.size.height - 40) {
            ((UILabel *)v).text = text;
        }
    }
}

- (void)setMenuAccentColor:(UIColor *)color {
    self.accentColor = color;
    [self updateTheme];
}

- (void)setMenuGlassEffect:(BOOL)enabled {
    self.blurEffectView.hidden = !enabled;
}

- (void)setMenuCornerRadius:(CGFloat)radius {
    self.layer.cornerRadius = radius;
}

- (void)setMenuBorderWidth:(CGFloat)width {
    self.layer.borderWidth = width;
}

- (void)setMenuTitle:(NSString *)title {
    self.titleLabel.text = title;
}

- (void)setMenuSubtitle:(NSString *)subtitle {
    self.subtitleLabel.text = subtitle;
}

- (void)updateTheme {
    for (UISwitch *s in self.switches.allValues) {
        s.onTintColor = self.accentColor;
    }
    for (UISlider *sl in self.sliders.allValues) {
        sl.minimumTrackTintColor = self.accentColor;
    }
    for (UILabel *lbl in self.sliderLabels.allValues) {
        lbl.textColor = self.accentColor;
    }
    for (UIButton *btn in self.buttons.allValues) {
        if (![btn.currentTitle isEqualToString:@"✕"]) {
            btn.backgroundColor = self.accentColor;
        }
    }
}

- (void)tabChanged:(UISegmentedControl *)sender {
    [self endEditing:YES];
    [self updateLayout];
}

- (void)addSectionTitle:(NSString *)title {
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.frame.size.width, 25)];
    container.tag = self.currentCategoryCounter + 1000;
    
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(20, 5, 0, 20)];
    label.text = [title uppercaseString];
    label.textColor = [UIColor colorWithWhite:0.6 alpha:1.0];
    label.font = [UIFont systemFontOfSize:10 weight:UIFontWeightBold];
    CGSize textSize = [label sizeThatFits:CGSizeMake(self.frame.size.width - 40, 20)];
    label.frame = CGRectMake(20, 5, textSize.width, 20);
    [container addSubview:label];
    
    UIView *line = [[UIView alloc] initWithFrame:CGRectMake(20 + textSize.width + 10, 15, self.frame.size.width - (20 + textSize.width + 10) - 20, 1)];
    line.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.05];
    [container addSubview:line];
    
    [self.contentView addSubview:container];
    [self updateLayout];
}

- (void)setTabIndex:(NSInteger)index {
    self.currentCategoryCounter = index;
}

- (void)addFeatureSwitch:(NSString *)title description:(NSString *)desc handler:(void (^)(BOOL isOn))handler {
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(20, 0, self.frame.size.width - 40, 50)];
    container.tag = self.currentCategoryCounter + 1000;
    
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(0, 5, container.frame.size.width - 60, 20)];
    label.text = title;
    label.textColor = [UIColor whiteColor];
    label.font = [UIFont systemFontOfSize:15 weight:UIFontWeightSemibold];
    [container addSubview:label];
    
    UILabel *descLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 25, container.frame.size.width - 60, 15)];
    descLabel.text = desc;
    descLabel.textColor = [UIColor colorWithWhite:0.6 alpha:1.0];
    descLabel.font = [UIFont systemFontOfSize:11];
    [container addSubview:descLabel];
    
    UISwitch *toggle = [[UISwitch alloc] initWithFrame:CGRectMake(container.frame.size.width - 51, 10, 51, 31)];
    toggle.onTintColor = self.accentColor;
    [toggle addTarget:self action:@selector(switchChanged:) forControlEvents:UIControlEventValueChanged];
    objc_setAssociatedObject(toggle, "switchHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [container addSubview:toggle];
    
    [self.contentView addSubview:container];
    self.switches[title] = toggle;
    [self updateLayout];
}

- (void)addSlider:(NSString *)title max:(CGFloat)max min:(CGFloat)min value:(CGFloat)value handler:(void (^)(CGFloat value))handler {
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(20, 0, self.frame.size.width - 40, 60)];
    container.tag = self.currentCategoryCounter + 1000;
    
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, container.frame.size.width - 100, 20)];
    titleLabel.text = title;
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.font = [UIFont systemFontOfSize:14 weight:UIFontWeightMedium];
    [container addSubview:titleLabel];
    
    UILabel *valueLabel = [[UILabel alloc] initWithFrame:CGRectMake(container.frame.size.width - 50, 0, 50, 20)];
    valueLabel.textColor = self.accentColor;
    valueLabel.font = [UIFont systemFontOfSize:13 weight:UIFontWeightBold];
    valueLabel.textAlignment = NSTextAlignmentRight;
    valueLabel.text = [NSString stringWithFormat:@"%.1f", value];
    [container addSubview:valueLabel];
    
    UISlider *slider = [[UISlider alloc] initWithFrame:CGRectMake(0, 25, container.frame.size.width, 30)];
    slider.minimumValue = min;
    slider.maximumValue = max;
    slider.value = value;
    slider.minimumTrackTintColor = self.accentColor;
    [slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
    objc_setAssociatedObject(slider, "sliderHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [container addSubview:slider];
    
    [self.contentView addSubview:container];
    self.sliders[title] = slider;
    self.sliderLabels[title] = valueLabel;
    [self updateLayout];
}

- (void)addComboSelector:(NSString *)title options:(NSArray *)options selectedIndex:(NSInteger)index handler:(void (^)(NSInteger selectedIndex))handler {
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(20, 0, self.frame.size.width - 40, 70)];
    container.tag = self.currentCategoryCounter + 1000;
    
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, container.frame.size.width, 20)];
    titleLabel.text = title;
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.font = [UIFont systemFontOfSize:14 weight:UIFontWeightMedium];
    [container addSubview:titleLabel];
    
    UIButton *comboBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    comboBtn.frame = CGRectMake(0, 25, container.frame.size.width, 40);
    comboBtn.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.05];
    comboBtn.layer.cornerRadius = 12;
    comboBtn.layer.borderWidth = 1.0;
    comboBtn.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.1].CGColor;
    [comboBtn setTitle:options[index] forState:UIControlStateNormal];
    [comboBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    comboBtn.titleLabel.font = [UIFont systemFontOfSize:13];
    comboBtn.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
    comboBtn.titleEdgeInsets = UIEdgeInsetsMake(0, 15, 0, 0);
    
    // Arrow icon
    UILabel *arrow = [[UILabel alloc] initWithFrame:CGRectMake(comboBtn.frame.size.width - 30, 0, 20, 40)];
    arrow.text = @"▼";
    arrow.textColor = [UIColor colorWithWhite:0.5 alpha:1.0];
    arrow.font = [UIFont systemFontOfSize:10];
    [comboBtn addSubview:arrow];
    
    [comboBtn addTarget:self action:@selector(comboTapped:) forControlEvents:UIControlEventTouchUpInside];
    objc_setAssociatedObject(comboBtn, "comboOptions", options, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    objc_setAssociatedObject(comboBtn, "comboHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    
    [container addSubview:comboBtn];
    [self.contentView addSubview:container];
    [self updateLayout];
}

- (void)comboTapped:(UIButton *)sender {
    NSArray *options = objc_getAssociatedObject(sender, "comboOptions");
    void (^handler)(NSInteger) = objc_getAssociatedObject(sender, "comboHandler");
    
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Chọn lựa chọn" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    for (NSInteger i = 0; i < options.count; i++) {
        [alert addAction:[UIAlertAction actionWithTitle:options[i] style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            [sender setTitle:options[i] forState:UIControlStateNormal];
            if (handler) handler(i);
        }]];
    }
    [alert addAction:[UIAlertAction actionWithTitle:@"Hủy" style:UIAlertActionStyleCancel handler:nil]];
    
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    UIViewController *root = window.rootViewController;
    [root presentViewController:alert animated:YES completion:nil];
}

- (void)addTextField:(NSString *)title placeholder:(NSString *)placeholder handler:(void (^)(NSString *text))handler {
    UIView *container = [[UIView alloc] initWithFrame:CGRectMake(20, 0, self.frame.size.width - 40, 70)];
    container.tag = self.currentCategoryCounter + 1000;
    
    UILabel *titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(0, 0, container.frame.size.width, 20)];
    titleLabel.text = title;
    titleLabel.textColor = [UIColor whiteColor];
    titleLabel.font = [UIFont systemFontOfSize:14 weight:UIFontWeightMedium];
    [container addSubview:titleLabel];
    
    UITextField *field = [[UITextField alloc] initWithFrame:CGRectMake(0, 25, container.frame.size.width, 40)];
    field.backgroundColor = [UIColor colorWithWhite:1.0 alpha:0.05];
    field.layer.cornerRadius = 12;
    field.layer.borderWidth = 1.0;
    field.layer.borderColor = [UIColor colorWithWhite:1.0 alpha:0.1].CGColor;
    field.textColor = [UIColor whiteColor];
    field.font = [UIFont systemFontOfSize:13];
    field.attributedPlaceholder = [[NSAttributedString alloc] initWithString:placeholder attributes:@{NSForegroundColorAttributeName: [UIColor colorWithWhite:1.0 alpha:0.4]}];
    field.delegate = self;
    field.returnKeyType = UIReturnKeyDone;
    
    // Padding
    UIView *paddingView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 15, 40)];
    field.leftView = paddingView;
    field.leftViewMode = UITextFieldViewModeAlways;
    
    objc_setAssociatedObject(field, "fieldHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [container addSubview:field];
    [self.contentView addSubview:container];
    [self updateLayout];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    void (^handler)(NSString *) = objc_getAssociatedObject(textField, "fieldHandler");
    if (handler) handler(textField.text);
    return YES;
}

- (void)addButton:(NSString *)title withHandler:(void (^)(void))handler {
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.frame = CGRectMake(20, 0, self.frame.size.width - 40, 45);
    button.tag = self.currentCategoryCounter + 1000;
    [button setTitle:[title uppercaseString] forState:UIControlStateNormal];
    [button setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    button.titleLabel.font = [UIFont systemFontOfSize:14 weight:UIFontWeightBold];
    button.backgroundColor = [UIColor colorWithRed:110.0/255.0 green:142.0/255.0 blue:251.0/255.0 alpha:1.0];
    button.layer.cornerRadius = 14;
    objc_setAssociatedObject(button, "buttonHandler", handler, OBJC_ASSOCIATION_COPY_NONATOMIC);
    [button addTarget:self action:@selector(buttonTapped:) forControlEvents:UIControlEventTouchUpInside];
    [self.contentView addSubview:button];
    self.buttons[title] = button;
    [self updateLayout];
}

- (void)updateLayout {
    NSInteger selectedTab = self.tabBar.selectedSegmentIndex;
    __block CGFloat yOffset = 10;
    [self.contentView.subviews enumerateObjectsUsingBlock:^(__kindof UIView * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if (obj.tag >= 1000) {
            if (obj.tag - 1000 == selectedTab) {
                obj.hidden = NO;
                CGRect frame = obj.frame;
                frame.origin.y = yOffset;
                obj.frame = frame;
                yOffset += frame.size.height + 8;
            } else { obj.hidden = YES; }
        }
    }];
    self.contentView.frame = CGRectMake(0, 0, self.frame.size.width, yOffset);
    self.scrollView.contentSize = CGSizeMake(self.scrollView.frame.size.width, yOffset);
}

- (void)switchChanged:(UISwitch *)sender {
    void (^handler)(BOOL) = objc_getAssociatedObject(sender, "switchHandler");
    if (handler) handler(sender.isOn);
}

- (void)sliderValueChanged:(UISlider *)slider {
    for (NSString *key in self.sliders) {
        if (self.sliders[key] == slider) {
            UILabel *label = (UILabel *)self.sliderLabels[key];
            label.text = [NSString stringWithFormat:@"%.1f", slider.value];
            
            void (^handler)(CGFloat) = objc_getAssociatedObject(slider, "sliderHandler");
            if (handler) handler(slider.value);

            NSString *prop = objc_getAssociatedObject(slider, "themeProp");
            if ([prop isEqualToString:@"opacity"]) self.alpha = slider.value;
            if ([prop isEqualToString:@"corner"]) [self setMenuCornerRadius:slider.value];
            if ([prop isEqualToString:@"border"]) [self setMenuBorderWidth:slider.value];
            break;
        }
    }
}

- (void)buttonTapped:(UIButton *)sender {
    [UIView animateWithDuration:0.1 animations:^{ sender.transform = CGAffineTransformMakeScale(0.96, 0.96); } completion:^(BOOL finished) {
        [UIView animateWithDuration:0.1 animations:^{ sender.transform = CGAffineTransformIdentity; } completion:^(BOOL finished) {
            void (^handler)(void) = objc_getAssociatedObject(sender, "buttonHandler");
            if (handler) handler();
        }];
    }];
}

- (void)close {
    [UIView animateWithDuration:0.3 animations:^{ self.alpha = 0; } completion:^(BOOL finished) { [self removeFromSuperview]; }];
}

- (void)makeDraggable {
    UIPanGestureRecognizer *pan = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePan:)];
    [self addGestureRecognizer:pan];
}

- (void)handlePan:(UIPanGestureRecognizer *)gesture {
    CGPoint translation = [gesture translationInView:self.superview];
    if (gesture.state == UIGestureRecognizerStateBegan) self.lastLocation = self.center;
    self.center = CGPointMake(self.lastLocation.x + translation.x, self.lastLocation.y + translation.y);
}

- (void)closeButtonTapped:(UIButton *)sender { [self close]; }
- (void)addFeatureSwitch:(NSString *)title { [self addFeatureSwitch:title description:@"Custom feature" handler:nil]; }

@end