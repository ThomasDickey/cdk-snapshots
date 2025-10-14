# $NetBSD: Makefile,v 1.49 2024/06/26 16:23:34 wiz Exp $

DISTNAME=		cdk-5.0-20251014
PKGNAME=		${DISTNAME:S/5.0-/5.0./}
PKGREVISION=		1
CATEGORIES=		devel
MASTER_SITES=		https://invisible-island.net/archives/cdk/ \
			https://invisible-mirror.net/archives/cdk/
EXTRACT_SUFX=		.tgz

MAINTAINER=		pkgsrc-users@NetBSD.org
HOMEPAGE=		https://invisible-island.net/cdk/
COMMENT=		Curses Development Kit
LICENSE=		x11

WRKSRC=			${WRKDIR}/${DISTNAME}
GNU_CONFIGURE=		yes
# The buildlink3.mk script is well-meaning but confuses the configure script by
# making it generate cdk5-config --libs as "-lcurses".  Don't try to fix the
# buildlink3.mk, but help the configure script find the right library name.
# Doing this will still leave breakage in the check for header files which must
# be worked around in anything that uses libraries built with buildlink3.mk
CONFIGURE_ARGS+=	--with-screen=ncurses
# Alternatively, the same PLIST but better handling of line-drawing:
# CONFIGURE_ARGS+=	--with-screen=ncursesw
# CONFIGURE_ARGS+=	--with-libname=cdk
CONFIGURE_ARGS+=	--enable-const
CONFIGURE_ARGS+=	--enable-pc-files
USE_LIBTOOL=		yes
USE_TOOLS+=		pkg-config

BUILD_TARGET=		default

INSTALL_MAKE_FLAGS+=	INSTALL_DIR=${DESTDIR}${PREFIX} \
			PKGCONFIG_DIR=${DESTDIR}${PREFIX}/lib/pkgconfig \
			MAN_DIR=${DESTDIR}${PREFIX}/${PKGMANDIR}/man3
INSTALLATION_DIRS=	share/examples/cdk

# put Cdk's include-directory at the front to avoid conflict with menu.h
CFLAGS+=		-I./include

post-install:
	for file in ${WRKSRC}/examples/*.c ${WRKSRC}/examples/Makefile; do \
		${INSTALL_DATA} $${file} ${DESTDIR}${PREFIX}/share/examples/cdk/; \
	done

.include "../../devel/ncurses/buildlink3.mk"
.include "../../mk/bsd.pkg.mk"
