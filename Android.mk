LOCAL_PATH := $(call my-dir)

# Copy only basic terminal type definitions as ncurses-base in debian does by default.
# https://salsa.debian.org/debian/ncurses/-/blob/master/debian/rules#L179
TERMINFO_FILES := \
	a/ansi c/cons25 c/cons25-debian c/cygwin d/dumb h/hurd l/linux \
	m/mach m/mach-bold m/mach-color m/mach-gnu m/mach-gnu-color p/pcansi \
	r/rxvt r/rxvt-basic \
	s/screen s/screen-256color s/screen-256color-bce s/screen-bce \
	s/screen-s s/screen-w s/screen.xterm-256color s/sun t/tmux \
	t/tmux-256color v/vt100 v/vt102 v/vt220 v/vt52 w/wsvt25 w/wsvt25m \
	x/xterm x/xterm-256color x/xterm-color x/xterm-mono \
	x/xterm-r5 x/xterm-r6 x/xterm-vt220 x/xterm-xfree86 E/Eterm \
	E/Eterm-color

TERMINFO_SOURCE := $(LOCAL_PATH)/lib/terminfo
TERMINFO_TARGET := $(TARGET_OUT_ETC)/terminfo
$(TERMINFO_TARGET): $(ACP)
	@echo "copy terminfo to /etc/"
	@mkdir -p $@
	@$(foreach TERMINFO_FILE,$(TERMINFO_FILES), \
		mkdir -p $@/$(dir $(TERMINFO_FILE)); \
		$(ACP) $(TERMINFO_SOURCE)/$(TERMINFO_FILE) $@/$(TERMINFO_FILE); \
	)
ALL_DEFAULT_INSTALLED_MODULES += $(TERMINFO_TARGET)
