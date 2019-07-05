LOCAL_PATH:= $(call my-dir)
# Copy only basic terminal type definitions as ncurses-base in debian does by default.
# http://anonscm.debian.org/gitweb/?p=collab-maint/ncurses.git;a=blob;f=debian/rules;hb=HEAD#l140
TERMINFO_FILES := \
	a/ansi c/cons25 c/cygwin d/dumb E/Eterm E/Eterm-color h/hurd l/linux \
	m/mach m/mach-bold m/mach-color p/pcansi r/rxvt r/rxvt-basic  \
	s/screen s/screen-bce s/screen-s s/screen-w s/sun \
	s/screen-256color s/screen-256color-bce v/vt100 v/vt102 \
	v/vt220 v/vt52 x/xterm x/xterm-xfree86 x/xterm-color x/xterm-r5 x/xterm-r6 \
	x/xterm-vt220 x/xterm-256color w/wsvt25 w/wsvt25m

TERMINFO_SOURCE := $(LOCAL_PATH)/lib/terminfo/
TERMINFO_TARGET := $(TARGET_OUT_ETC)/terminfo
$(TERMINFO_TARGET): $(ACP)
		@echo "copy terminfo to /etc/" 
		@mkdir -p $@
		@$(foreach TERMINFO_FILE,$(TERMINFO_FILES), \
			mkdir -p $@/$(dir $(TERMINFO_FILE)); \
			$(ACP) $(TERMINFO_SOURCE)/$(TERMINFO_FILE) $@/$(TERMINFO_FILE); \
		)
ALL_DEFAULT_INSTALLED_MODULES += $(TERMINFO_TARGET)
