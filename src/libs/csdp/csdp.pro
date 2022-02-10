include(../../ligerlibrary.pri)

#DEFINES += CSDP_LIBRARY

CONFIG -= qt
INCLUDEPATH += $$PWD

HEADERS += \
    declarations.h \
    csdp_global.h \
    blockmat.h \
    index.h \
    parameters.h

SOURCES += \
      Fnorm.c \
      add_mat.c \
      addscaledmat.c \
      allocmat.c \
      calc_dobj.c \
      calc_pobj.c \
      chol.c \
      copy_mat.c \
      easysdp.c \
      freeprob.c \
      initparams.c \
      initsoln.c \
      linesearch.c \
      make_i.c \
      makefill.c \
      mat_mult.c \
      mat_multsp.c \
      matvec.c \
      norms.c \
      op_a.c \
      op_at.c \
      op_o.c \
      packed.c \
      psd_feas.c \
      qreig.c \
      readprob.c \
      readsol.c \
      sdp.c \
      solvesys.c \
      sortentries.c \
      sym_mat.c \
      trace_prod.c \
      tweakgap.c \
      user_exit.c \
      writeprob.c \
      writesol.c \
      zero_mat.c

linux-* {
    LIBS += -lblas -llapack -lopenblas -lgfortran
}
