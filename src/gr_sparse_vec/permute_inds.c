#include <stdlib.h>
#include "gr_sparse_vec.h"

typedef struct
{
    slong col;
    slong i;
}
gr_sparse_vec_slong_sorter_t;

static int gr_sparse_vec_slong_sort(const void* a, const void* b)
{
    slong ak = ((gr_sparse_vec_slong_sorter_t*)(a))->col;
    slong bk = ((gr_sparse_vec_slong_sorter_t*)(b))->col;
    return (ak < bk ? -1 : (ak > bk ? 1 : 0));
}

int
gr_sparse_vec_permute_inds(gr_sparse_vec_t vec, const gr_sparse_vec_t src, slong * p, gr_ctx_t ctx)
{
    slong i,sz,nnz;
    int status = GR_SUCCESS;
    gr_sparse_vec_slong_sorter_t *si;
    if (vec == src)
    {
        gr_sparse_vec_t temp;
        gr_sparse_vec_init(temp, src->length, ctx);
        GR_MUST_SUCCEED(gr_sparse_vec_set(temp, src, ctx));
        status |= gr_sparse_vec_permute_inds(temp, src, p, ctx);
        status |= gr_sparse_vec_set(vec, temp, ctx);
        gr_sparse_vec_clear(temp, ctx);
        return status;
    }
    sz = ctx->sizeof_elem;
    nnz = src->nnz;
    si = flint_malloc(nnz * sizeof(gr_sparse_vec_slong_sorter_t));
    for (i = 0; i < nnz; i++)
    {
        si[i].col = p[vec->inds[i]];
        si[i].i = i;
    }
    qsort(si, nnz, sizeof(gr_sparse_vec_slong_sorter_t), gr_sparse_vec_slong_sort);
    for (i = 0; i < nnz; i++)
    {
        vec->inds[i] = si[i].col;
        status |= gr_set(GR_ENTRY(vec->nzs, i, sz), GR_ENTRY(src->nzs, si[i].i, sz), ctx);
    }
    flint_free(si);
    return status;
}
