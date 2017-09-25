### Segmentation based on contour recognition and ellipse fitting

This simple script takes a trans image of budding yeast. It has been developed using a test set of ten images of DIC microscopy. 
It tends to overfit. It is also picky, because of the heuristic that false negatives are fare more of a hindrance to our analysis than false positives.

Needs to classify ellipses to a single ellipse to reach the one cell = one crop ratio. This will allow for automated assays in the future.
