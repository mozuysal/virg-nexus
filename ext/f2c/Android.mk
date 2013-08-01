include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../jni/virg_nexus/ext/include
LOCAL_CFLAGS := -DVIRG_NEXUS_ANDROID

LOCAL_MODULE := virg_nexus_f2c
LOCAL_SRC_FILES := virg_nexus/ext/f2c/abort_.c    virg_nexus/ext/f2c/d_cnjg.c  virg_nexus/ext/f2c/d_sinh.c                      virg_nexus/ext/f2c/l_lt.c      virg_nexus/ext/f2c/r_asin.c  virg_nexus/ext/f2c/r_sin.c    virg_nexus/ext/f2c/system_.c \
virg_nexus/ext/f2c/d_cos.c   virg_nexus/ext/f2c/d_sqrt.c    virg_nexus/ext/f2c/ftell_.c    virg_nexus/ext/f2c/i_ceiling.c   virg_nexus/ext/f2c/lread.c     virg_nexus/ext/f2c/r_atan.c  virg_nexus/ext/f2c/r_sinh.c   virg_nexus/ext/f2c/typesize.c \
virg_nexus/ext/f2c/backspac.c  virg_nexus/ext/f2c/d_cosh.c  virg_nexus/ext/f2c/d_tan.c                     virg_nexus/ext/f2c/i_dim.c       virg_nexus/ext/f2c/lwrite.c    virg_nexus/ext/f2c/r_atn2.c  virg_nexus/ext/f2c/rsli.c     virg_nexus/ext/f2c/uio.c \
virg_nexus/ext/f2c/c_abs.c     virg_nexus/ext/f2c/d_dim.c   virg_nexus/ext/f2c/d_tanh.c    virg_nexus/ext/f2c/getenv_.c   virg_nexus/ext/f2c/i_dnnt.c      virg_nexus/ext/f2c/r_cnjg.c  virg_nexus/ext/f2c/rsne.c      \
virg_nexus/ext/f2c/cabs.c      virg_nexus/ext/f2c/derf_.c   virg_nexus/ext/f2c/dtime_.c    virg_nexus/ext/f2c/h_abs.c     virg_nexus/ext/f2c/i_indx.c      virg_nexus/ext/f2c/open.c      virg_nexus/ext/f2c/r_cos.c   virg_nexus/ext/f2c/r_sqrt.c   virg_nexus/ext/f2c/util.c \
virg_nexus/ext/f2c/c_cos.c     virg_nexus/ext/f2c/derfc_.c  virg_nexus/ext/f2c/due.c       virg_nexus/ext/f2c/h_dim.c     virg_nexus/ext/f2c/iio.c         virg_nexus/ext/f2c/pow_ci.c    virg_nexus/ext/f2c/r_cosh.c  virg_nexus/ext/f2c/r_tan.c    virg_nexus/ext/f2c/wref.c \
virg_nexus/ext/f2c/c_div.c     virg_nexus/ext/f2c/d_exp.c   virg_nexus/ext/f2c/ef1asc_.c   virg_nexus/ext/f2c/h_dnnt.c    virg_nexus/ext/f2c/i_len.c       virg_nexus/ext/f2c/pow_dd.c    virg_nexus/ext/f2c/rdfmt.c   virg_nexus/ext/f2c/r_tanh.c   virg_nexus/ext/f2c/wrtfmt.c \
virg_nexus/ext/f2c/c_exp.c     virg_nexus/ext/f2c/dfe.c     virg_nexus/ext/f2c/ef1cmc_.c   virg_nexus/ext/f2c/h_indx.c    virg_nexus/ext/f2c/i_len_trim.c  virg_nexus/ext/f2c/pow_di.c    virg_nexus/ext/f2c/r_dim.c   virg_nexus/ext/f2c/s_cat.c    virg_nexus/ext/f2c/wsfe.c \
virg_nexus/ext/f2c/c_log.c     virg_nexus/ext/f2c/d_imag.c  virg_nexus/ext/f2c/endfile.c   virg_nexus/ext/f2c/h_len.c     virg_nexus/ext/f2c/ilnw.c        virg_nexus/ext/f2c/pow_hh.c    virg_nexus/ext/f2c/rewind.c  virg_nexus/ext/f2c/s_cmp.c    virg_nexus/ext/f2c/wsle.c \
virg_nexus/ext/f2c/close.c     virg_nexus/ext/f2c/d_int.c   virg_nexus/ext/f2c/erf_.c      virg_nexus/ext/f2c/hl_ge.c     virg_nexus/ext/f2c/i_mod.c       virg_nexus/ext/f2c/pow_ii.c    virg_nexus/ext/f2c/r_exp.c   virg_nexus/ext/f2c/s_copy.c   virg_nexus/ext/f2c/wsne.c \
virg_nexus/ext/f2c/c_sin.c     virg_nexus/ext/f2c/d_lg10.c  virg_nexus/ext/f2c/erfc_.c     virg_nexus/ext/f2c/hl_gt.c     virg_nexus/ext/f2c/i_nint.c      virg_nexus/ext/f2c/r_imag.c    virg_nexus/ext/f2c/sfe.c     virg_nexus/ext/f2c/xwsne.c \
virg_nexus/ext/f2c/c_sqrt.c    virg_nexus/ext/f2c/d_log.c   virg_nexus/ext/f2c/err.c       virg_nexus/ext/f2c/hl_le.c     virg_nexus/ext/f2c/inquire.c     virg_nexus/ext/f2c/pow_ri.c    virg_nexus/ext/f2c/r_int.c   virg_nexus/ext/f2c/sig_die.c  virg_nexus/ext/f2c/z_abs.c \
virg_nexus/ext/f2c/ctype.c     virg_nexus/ext/f2c/d_mod.c   virg_nexus/ext/f2c/etime_.c    virg_nexus/ext/f2c/hl_lt.c     virg_nexus/ext/f2c/i_sign.c      virg_nexus/ext/f2c/pow_zi.c    virg_nexus/ext/f2c/r_lg10.c  virg_nexus/ext/f2c/signal_.c  virg_nexus/ext/f2c/z_cos.c \
virg_nexus/ext/f2c/d_abs.c     virg_nexus/ext/f2c/d_nint.c  virg_nexus/ext/f2c/exit_.c     virg_nexus/ext/f2c/h_mod.c     virg_nexus/ext/f2c/lbitbits.c    virg_nexus/ext/f2c/pow_zz.c    virg_nexus/ext/f2c/r_log.c   virg_nexus/ext/f2c/z_div.c \
virg_nexus/ext/f2c/d_acos.c    virg_nexus/ext/f2c/dolio.c   virg_nexus/ext/f2c/f77_aloc.c  virg_nexus/ext/f2c/h_nint.c    virg_nexus/ext/f2c/lbitshft.c    virg_nexus/ext/f2c/r_mod.c   virg_nexus/ext/f2c/s_paus.c   virg_nexus/ext/f2c/z_exp.c \
virg_nexus/ext/f2c/d_asin.c    virg_nexus/ext/f2c/d_prod.c  virg_nexus/ext/f2c/f77vers.c   virg_nexus/ext/f2c/h_sign.c    virg_nexus/ext/f2c/l_ge.c        virg_nexus/ext/f2c/r_nint.c  virg_nexus/ext/f2c/s_rnge.c   virg_nexus/ext/f2c/z_log.c \
virg_nexus/ext/f2c/d_atan.c    virg_nexus/ext/f2c/d_sign.c  virg_nexus/ext/f2c/fmt.c       virg_nexus/ext/f2c/i77vers.c   virg_nexus/ext/f2c/l_gt.c        virg_nexus/ext/f2c/r_abs.c     virg_nexus/ext/f2c/rsfe.c    virg_nexus/ext/f2c/s_stop.c   virg_nexus/ext/f2c/z_sin.c \
virg_nexus/ext/f2c/d_atn2.c    virg_nexus/ext/f2c/d_sin.c   virg_nexus/ext/f2c/fmtlib.c    virg_nexus/ext/f2c/i_abs.c     virg_nexus/ext/f2c/l_le.c        virg_nexus/ext/f2c/r_acos.c    virg_nexus/ext/f2c/r_sign.c  virg_nexus/ext/f2c/sue.c      virg_nexus/ext/f2c/z_sqrt.c

include $(BUILD_STATIC_LIBRARY)
