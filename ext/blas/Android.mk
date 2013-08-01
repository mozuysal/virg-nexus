include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../jni/virg_nexus/ext/include
LOCAL_MODULE := virg_nexus_blas
LOCAL_SRC_FILES := virg_nexus/ext/blas/dsdot.c   virg_nexus/ext/blas/dtrsm.c   virg_nexus/ext/blas/sgbmv.c   virg_nexus/ext/blas/ssyr2.c         \
virg_nexus/ext/blas/dasum.c   virg_nexus/ext/blas/dspmv.c   virg_nexus/ext/blas/dtrsv.c   virg_nexus/ext/blas/sgemm.c   virg_nexus/ext/blas/ssyr2k.c        \
virg_nexus/ext/blas/daxpy.c   virg_nexus/ext/blas/dspr2.c   virg_nexus/ext/blas/dzasum.c  virg_nexus/ext/blas/sgemv.c   virg_nexus/ext/blas/ssyr.c          \
virg_nexus/ext/blas/dcabs1.c  virg_nexus/ext/blas/dspr.c    virg_nexus/ext/blas/dznrm2.c  virg_nexus/ext/blas/sger.c    virg_nexus/ext/blas/ssyrk.c         \
virg_nexus/ext/blas/dcopy.c   virg_nexus/ext/blas/dswap.c   virg_nexus/ext/blas/icamax.c  virg_nexus/ext/blas/snrm2.c   virg_nexus/ext/blas/stbmv.c         \
virg_nexus/ext/blas/ddot.c    virg_nexus/ext/blas/dsymm.c   virg_nexus/ext/blas/idamax.c  virg_nexus/ext/blas/srot.c    virg_nexus/ext/blas/stbsv.c         \
virg_nexus/ext/blas/dgbmv.c   virg_nexus/ext/blas/dsymv.c   virg_nexus/ext/blas/isamax.c  virg_nexus/ext/blas/srotg.c   virg_nexus/ext/blas/stpmv.c         \
virg_nexus/ext/blas/dgemm.c   virg_nexus/ext/blas/dsyr2.c   virg_nexus/ext/blas/izamax.c  virg_nexus/ext/blas/srotm.c   virg_nexus/ext/blas/stpsv.c         \
virg_nexus/ext/blas/dgemv.c   virg_nexus/ext/blas/dsyr2k.c  virg_nexus/ext/blas/lsame.c   virg_nexus/ext/blas/srotmg.c  virg_nexus/ext/blas/strmm.c         \
virg_nexus/ext/blas/dger.c    virg_nexus/ext/blas/dsyr.c    virg_nexus/ext/blas/sasum.c   virg_nexus/ext/blas/ssbmv.c   virg_nexus/ext/blas/strmv.c         \
virg_nexus/ext/blas/dnrm2.c   virg_nexus/ext/blas/dsyrk.c   virg_nexus/ext/blas/saxpy.c   virg_nexus/ext/blas/sscal.c   virg_nexus/ext/blas/strsm.c         \
virg_nexus/ext/blas/drot.c    virg_nexus/ext/blas/dtbmv.c   virg_nexus/ext/blas/scabs1.c  virg_nexus/ext/blas/sspmv.c   virg_nexus/ext/blas/strsv.c         \
virg_nexus/ext/blas/drotg.c   virg_nexus/ext/blas/dtbsv.c   virg_nexus/ext/blas/scasum.c  virg_nexus/ext/blas/sspr2.c   virg_nexus/ext/blas/xerbla_array.c  \
virg_nexus/ext/blas/drotm.c   virg_nexus/ext/blas/dtpmv.c   virg_nexus/ext/blas/scnrm2.c  virg_nexus/ext/blas/sspr.c    virg_nexus/ext/blas/xerbla.c        \
virg_nexus/ext/blas/drotmg.c  virg_nexus/ext/blas/dtpsv.c   virg_nexus/ext/blas/scopy.c   virg_nexus/ext/blas/sswap.c   \
virg_nexus/ext/blas/dsbmv.c   virg_nexus/ext/blas/dtrmm.c   virg_nexus/ext/blas/sdot.c    virg_nexus/ext/blas/ssymm.c   \
virg_nexus/ext/blas/dscal.c   virg_nexus/ext/blas/dtrmv.c   virg_nexus/ext/blas/sdsdot.c  virg_nexus/ext/blas/ssymv.c

include $(BUILD_STATIC_LIBRARY)
