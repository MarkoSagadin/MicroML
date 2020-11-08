#ifndef MODEL_SETTINGS_H
#define MODEL_SETTINGS_H

// Keeping these as constant expressions allow us to allocate fixed-sized arrays
// on the stack for our working memory.

// All of these values are derived from the values used during model training,
// if you change your model you'll need to update these constants.
constexpr int kNumCols = 80;
constexpr int kNumRows = 60;
constexpr int kNumChannels = 1;

constexpr int kMaxImageSize = kNumCols * kNumRows * kNumChannels;

constexpr int kCategoryCount = 4;

const char* kCategoryLabels[kCategoryCount] = {
    "elephant",
    "human",
    "cow",
    "uknown/nature"
};

#endif  // MODEL_SETTINGS_H

