#
# Copyright (C) 2012-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ubus_object
PKG_VERSION:=1.0.0


PKG_SOURCE_URL:=https://github.com/jimenezrick/magic-makefile.git
PKG_HASH:=eb1a561bafc3db29c7d673b6fd5c8aeba38cf90d


# PKG_CONFIG_DEPENDS:= \
# 	CONFIG_EASYCWMP_DEBUG \
# 	CONFIG_EASYCWMP_DEVEL

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/ubus_object
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ubus object
  DEPENDS:=+libubus +libuci +libubox +libmicroxml +libjson-c +libcurl
endef

define Package/ubus_object/description
	An open source implementation hello_world
endef


TARGET_CFLAGS += \
	-D_GNU_SOURCE

TARGET_LDFLAGS += \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

CONFIGURE_ARGS += \
	--with-uci-include-path=$(STAGING_DIR)/usr/include \
	--with-libubox-include-path=$(STAGING_DIR)/usr/include \
	--with-libubus-include-path=$(STAGING_DIR)/usr/include

define Package/ubus_object/install

endef

$(eval $(call BuildPackage,ubus_object))
