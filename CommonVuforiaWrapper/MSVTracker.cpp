#include "MSVController.h"
#include "MSVTracker.h"

#include <string.h>

#include <QCAR/DataSet.h>
#include <QCAR/ImageTarget.h>
#include <QCAR/TrackerManager.h>

MSVTracker::MSVTracker() :
dataset_nb(0),
dataset_capacity(INITIAL_DATASET_NUMBER)
{
  QCAR::TrackerManager& trackerManager = QCAR::TrackerManager::getInstance();
  trackerManager.initTracker(QCAR::Tracker::IMAGE_TRACKER);
  datasets = (QCAR::DataSet **)calloc(INITIAL_DATASET_NUMBER, sizeof(QCAR::DataSet *));
  names = (char **)calloc(INITIAL_DATASET_NUMBER, sizeof(char *));
}

MSVTracker::~MSVTracker()
{
  for (int i = 0; i < dataset_nb; ++i) {
    datasetDestroy(this->datasets[i]);
    this->datasets[i] = NULL;
    free(this->names[i]);
    this->names[i] = NULL;
  }
  free(datasets);
  free(names);
  QCAR::TrackerManager& trackerManager = QCAR::TrackerManager::getInstance();
  trackerManager.deinitTracker(QCAR::Tracker::IMAGE_TRACKER);
}

void
MSVTracker::addDataset(const char *dataset)
{
  QCAR::ImageTracker* imageTracker = getTracker();
  int len = strlen(dataset);
  char *filename = (char *)malloc(len+5);
  memcpy(filename, dataset, len);
  memcpy(filename+len, ".xml\0", 5);
  if (QCAR::DataSet::exists(filename, QCAR::DataSet::STORAGE_APPRESOURCE)) {
    if (dataset_nb == dataset_capacity) {
      dataset_capacity *= 2;
      datasets = (QCAR::DataSet **)realloc(datasets, dataset_capacity*sizeof(QCAR::DataSet *));
      names = (char **)realloc(names, dataset_capacity*sizeof(char *));
    }
    datasets[dataset_nb] = imageTracker->createDataSet();
    datasets[dataset_nb]->load(filename, QCAR::DataSet::STORAGE_APPRESOURCE);
    datasetInit(datasets[dataset_nb]);
    names[dataset_nb] = strdup(dataset);
    dataset_nb++;
  }
  free(filename);
}

bool
MSVTracker::has(const char *name,
               const char *dataset) const
{
  int d_idx = -1;
  for (int i = 0; i < dataset_nb; ++i) {
    if (!strcmp(dataset, names[i])) {
      d_idx = i;
      break;
    }
  }
  if (d_idx < 0) return false;
  int t_idx = -1;
  QCAR::DataSet *d = datasets[d_idx];
  for (int i = 0; i < d->getNumTrackables(); ++i) {
    if (!strcmp(name, d->getTrackable(i)->getName())) {
      t_idx = i;
      break;
    }
  }
  if (t_idx < 0) return false;
  return true;
}

void
MSVTracker::start(const char *dataset) const
{
  QCAR::ImageTracker* imageTracker = getTracker();
  if (imageTracker != 0) {
    int d_idx = -1;
    for (int i = 0; i < dataset_nb; ++i) {
      if (!strcmp(dataset, names[i])) {
        d_idx = i;
        break;
      }
    }
    if (d_idx < 0) return;
    imageTracker->activateDataSet(datasets[d_idx]);
    imageTracker->start();
  }
}

void
MSVTracker::stop() const
{
    QCAR::ImageTracker *imageTracker = getTracker();
    if (imageTracker) {
      QCAR::DataSet *active = imageTracker->getActiveDataSet();
      if (active) datasetDeactivate(active);
      imageTracker->stop();
    }
}

void
MSVTracker::datasetInit(QCAR::DataSet *dataset)
{
  for (int i = 0; i < dataset->getNumTrackables(); ++i) {
    QCAR::ImageTarget *t = static_cast<QCAR::ImageTarget *>(
      dataset->getTrackable(i)
    );
    QCAR::Vec2F s = t->getSize();
    float w = s.data[0];
    float h = s.data[1];
    if (w > h) {
      w *= 2.0/h;
      h = 2.0;
    }
    else {
      h *= 2.0/w;
      w = 2.0;
    }
    t->setSize(QCAR::Vec2F(w,h));
  }
}

void
MSVTracker::datasetDeactivate(QCAR::DataSet *dataset)
{
  QCAR::ImageTracker *imageTracker = getTracker();
  if (dataset->isActive()) {
    imageTracker->deactivateDataSet(dataset);
  }
}

void
MSVTracker::datasetDestroy(QCAR::DataSet *dataset)
{
  QCAR::ImageTracker* imageTracker = getTracker();
  datasetDeactivate(dataset);
  imageTracker->destroyDataSet(dataset);
}

QCAR::ImageTracker *
MSVTracker::getTracker()
{
  QCAR::TrackerManager& trackerManager = QCAR::TrackerManager::getInstance();
  return static_cast<QCAR::ImageTracker*> (
    trackerManager.getTracker(QCAR::Tracker::IMAGE_TRACKER)
  );
}
