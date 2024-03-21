#include "gr_sparse_vec.h"

int
gr_sparse_vec_set_vec(gr_sparse_vec_t vec, gr_srcptr src, slong len, gr_ctx_t ctx)
{
    slong nnz,i,sz,status;
    sz = ctx->sizeof_elem;
    nnz = 0;
    status = GR_SUCCESS;
    for (i = 0; i < len; i++)
    {
        if (T_TRUE != gr_is_zero(GR_ENTRY(src, i, sz), ctx))
            nnz++;
    }
    gr_sparse_vec_fit_nnz(vec, nnz, ctx);
    nnz = 0;
    for (i = 0; i < len; i++)
    {
        if (T_TRUE != gr_is_zero(GR_ENTRY(src, i, sz), ctx))
        {
            vec->inds[nnz] = i;
            status |= gr_set(GR_ENTRY(vec->nzs, nnz, sz), GR_ENTRY(src, i, sz), ctx);
            nnz++;
        }
    }
    vec->length = len;
    vec->nnz = nnz;
    return status;
}
