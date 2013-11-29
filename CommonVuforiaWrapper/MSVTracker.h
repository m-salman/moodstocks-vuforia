#ifndef MSV_TRACKER_H
#define MSV_TRACKER_H

#include <QCAR/ImageTracker.h>

#define INITIAL_DATASET_NUMBER 1

class MSVTargetInfo;

/** Helper class around QCAR::ImageTracker */
class MSVTracker {

  public:
    MSVTracker();
    ~MSVTracker();

    /** Adds a bundled dataset produced with the Vuforia Target Manager to
     * the trackable datasets.
     * @param dataset the name of the dataset to add, corresponding to the
     *   `.dat` and `.xml` files generated using the Vuforia Target Manager.
     *   The dataset is loaded using STORAGE_APPRESOURCE flag, so these files
     *   should be bundled with the app.
     */
    void addDataset(const char *dataset);

    /** Checks that the name/dataset pair exists */
    bool has(const char *name,
             const char *dataset) const;

    /** Starts tracking from the specified dataset.
     * @param dataset the name of the dataset to add, corresponding to the
     *   `.dat` and `.xml` files generated using the Vuforia Target Manager.
     */
    void start(const char *dataset) const;

    /** Stops tracking */
    void stop() const;

  private:
    QCAR::DataSet **datasets;
    char **names;
    int dataset_nb;
    int dataset_capacity;

    /** Rescales all targets so that they fit the convention used for
     * targets: greatest dimensions = 2.
     */
    static void datasetInit(QCAR::DataSet *dataset);
    static void datasetDestroy(QCAR::DataSet *dataset);
    static void datasetDeactivate(QCAR::DataSet *dataset);
    static QCAR::ImageTracker *getTracker();
};
#endif
