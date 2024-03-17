#include "gr_sparse_vec.h"

int
gr_sparse_vec_set(gr_sparse_vec_t res, const gr_sparse_vec_t src, gr_ctx_t ctx)
{
    int status = GR_SUCCESS;
    if (res != src)
    {
        slong nnz = src->nnz;
        gr_sparse_vec_fit_nnz(res, nnz, ctx);
        memcpy(res->inds, src->inds, nnz*sizeof(slong));
        status = _gr_vec_set(res->entries, src->entries, nnz, ctx);
        res->nnz = nnz;
        res->length = src->length;
    }
    return status;
}
