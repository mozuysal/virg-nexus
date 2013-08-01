include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../jni/virg_nexus/ext/include
LOCAL_MODULE := virg_nexus_clapack
LOCAL_SRC_FILES := virg_nexus/ext/clapack/sgesvd.c virg_nexus/ext/clapack/sgetrf.c virg_nexus/ext/clapack/sgetri.c virg_nexus/ext/clapack/sgetrs.c virg_nexus/ext/clapack/sgesv.c virg_nexus/ext/clapack/sposv.c \
virg_nexus/ext/clapack/sgelss.c virg_nexus/ext/clapack/spotrf.c virg_nexus/ext/clapack/spotri.c virg_nexus/ext/clapack/spotrs.c virg_nexus/ext/clapack/spotf2.c \
virg_nexus/ext/clapack/slaswp.c virg_nexus/ext/clapack/srscl.c  virg_nexus/ext/clapack/slabad.c virg_nexus/ext/clapack/sgetf2.c \
virg_nexus/ext/clapack/strtri.c virg_nexus/ext/clapack/slauum.c virg_nexus/ext/clapack/strti2.c virg_nexus/ext/clapack/slauu2.c \
virg_nexus/ext/clapack/slange.c virg_nexus/ext/clapack/sgebrd.c virg_nexus/ext/clapack/sgelqf.c virg_nexus/ext/clapack/sormbr.c virg_nexus/ext/clapack/sgeqrf.c \
virg_nexus/ext/clapack/slabrd.c virg_nexus/ext/clapack/sgelq2.c virg_nexus/ext/clapack/slarfg.c virg_nexus/ext/clapack/sormlq.c virg_nexus/ext/clapack/sormqr.c virg_nexus/ext/clapack/slassq.c \
virg_nexus/ext/clapack/slacpy.c virg_nexus/ext/clapack/slaset.c virg_nexus/ext/clapack/sorgbr.c virg_nexus/ext/clapack/sorgqr.c virg_nexus/ext/clapack/sbdsqr.c \
virg_nexus/ext/clapack/sorml2.c virg_nexus/ext/clapack/slapy2.c virg_nexus/ext/clapack/sorm2r.c virg_nexus/ext/clapack/sgeqr2.c virg_nexus/ext/clapack/sgebd2.c \
virg_nexus/ext/clapack/slartg.c virg_nexus/ext/clapack/slasr.c  virg_nexus/ext/clapack/slasv2.c virg_nexus/ext/clapack/slas2.c  virg_nexus/ext/clapack/slasq1.c \
virg_nexus/ext/clapack/sorgl2.c virg_nexus/ext/clapack/slasq4.c virg_nexus/ext/clapack/slasq5.c virg_nexus/ext/clapack/slasq6.c virg_nexus/ext/clapack/slaisnan.c \
virg_nexus/ext/clapack/slascl.c virg_nexus/ext/clapack/slasq2.c virg_nexus/ext/clapack/slasrt.c virg_nexus/ext/clapack/sorg2r.c virg_nexus/ext/clapack/slarf.c \
virg_nexus/ext/clapack/sorglq.c virg_nexus/ext/clapack/sisnan.c virg_nexus/ext/clapack/slasq3.c virg_nexus/ext/clapack/slarft.c virg_nexus/ext/clapack/slarfb.c \
virg_nexus/ext/clapack/slarfp.c virg_nexus/ext/clapack/slamch.c \
virg_nexus/ext/clapack/dgesvd.c virg_nexus/ext/clapack/dgetrf.c virg_nexus/ext/clapack/dgetri.c virg_nexus/ext/clapack/dgetrs.c virg_nexus/ext/clapack/dgesv.c virg_nexus/ext/clapack/dposv.c \
virg_nexus/ext/clapack/dgelss.c virg_nexus/ext/clapack/dpotrf.c virg_nexus/ext/clapack/dpotri.c virg_nexus/ext/clapack/dpotrs.c virg_nexus/ext/clapack/dpotf2.c \
virg_nexus/ext/clapack/dlaswp.c virg_nexus/ext/clapack/drscl.c  virg_nexus/ext/clapack/dlabad.c virg_nexus/ext/clapack/dgetf2.c \
virg_nexus/ext/clapack/dtrtri.c virg_nexus/ext/clapack/dlauum.c virg_nexus/ext/clapack/dtrti2.c virg_nexus/ext/clapack/dlauu2.c \
virg_nexus/ext/clapack/dlange.c virg_nexus/ext/clapack/dgebrd.c virg_nexus/ext/clapack/dgelqf.c virg_nexus/ext/clapack/dormbr.c virg_nexus/ext/clapack/dgeqrf.c \
virg_nexus/ext/clapack/dlabrd.c virg_nexus/ext/clapack/dgelq2.c virg_nexus/ext/clapack/dlarfg.c virg_nexus/ext/clapack/dormlq.c virg_nexus/ext/clapack/dormqr.c virg_nexus/ext/clapack/dlassq.c \
virg_nexus/ext/clapack/dlacpy.c virg_nexus/ext/clapack/dlaset.c virg_nexus/ext/clapack/dorgbr.c virg_nexus/ext/clapack/dorgqr.c virg_nexus/ext/clapack/dbdsqr.c \
virg_nexus/ext/clapack/dorml2.c virg_nexus/ext/clapack/dlapy2.c virg_nexus/ext/clapack/dorm2r.c virg_nexus/ext/clapack/dgeqr2.c virg_nexus/ext/clapack/dgebd2.c \
virg_nexus/ext/clapack/dlartg.c virg_nexus/ext/clapack/dlasr.c  virg_nexus/ext/clapack/dlasv2.c virg_nexus/ext/clapack/dlas2.c  virg_nexus/ext/clapack/dlasq1.c \
virg_nexus/ext/clapack/dorgl2.c virg_nexus/ext/clapack/dlasq4.c virg_nexus/ext/clapack/dlasq5.c virg_nexus/ext/clapack/dlasq6.c virg_nexus/ext/clapack/dlaisnan.c \
virg_nexus/ext/clapack/dlascl.c virg_nexus/ext/clapack/dlasq2.c virg_nexus/ext/clapack/dlasrt.c virg_nexus/ext/clapack/dorg2r.c virg_nexus/ext/clapack/dlarf.c \
virg_nexus/ext/clapack/dorglq.c virg_nexus/ext/clapack/disnan.c virg_nexus/ext/clapack/dlasq3.c virg_nexus/ext/clapack/dlarft.c virg_nexus/ext/clapack/dlarfb.c \
virg_nexus/ext/clapack/dlarfp.c virg_nexus/ext/clapack/dlamch.c \
virg_nexus/ext/clapack/ilaenv.c virg_nexus/ext/clapack/ilaslc.c virg_nexus/ext/clapack/ilaslr.c virg_nexus/ext/clapack/ieeeck.c virg_nexus/ext/clapack/iparmq.c \
virg_nexus/ext/clapack/iladlc.c virg_nexus/ext/clapack/iladlr.c

include $(BUILD_STATIC_LIBRARY)
