#ifndef PIXLAB_FILTER_H
#define PIXLAB_FILTER_H

class Image;

class Filter {
public:
    virtual ~Filter() = default;
    virtual void apply(Image& image) const = 0;
};

#endif
