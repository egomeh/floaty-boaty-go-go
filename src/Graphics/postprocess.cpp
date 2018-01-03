#include "postprocess.hpp"

bool PostProcess::operator==(const PostProcess & other)
{
    return order == other.order;
}

bool PostProcess::operator<(const PostProcess & other)
{
    return order < other.order;
}
