#include "../include/ClipProcessingJobs.h"

// Constructor responsible to choose processing type and apply to clip
ClipProcessingJobs::ClipProcessingJobs(std::string processingType, std::string processInfoJson) : 
processingType(processingType), processInfoJson(processInfoJson){
}

void ClipProcessingJobs::processClip(Clip& clip){

    // Process clip and save processed data
    if(processingType == "Stabilizer"){
        t = std::thread(&ClipProcessingJobs::stabilizeClip, this, std::ref(clip), std::ref(this->processingController));
    }
    if(processingType == "Tracker"){
        t = std::thread(&ClipProcessingJobs::trackClip, this, std::ref(clip), std::ref(this->processingController));
    }
}

// Apply object tracking to clip 
void ClipProcessingJobs::trackClip(Clip& clip, ProcessingController& controller){

    // Create CVTracker object
    CVTracker tracker(processInfoJson, controller);
    // Start tracking
    tracker.trackClip(clip);

    // Thread controller. If effect processing is done, save data
    // Else, kill thread
    if(controller.ShouldStop()){
        controller.SetFinished(true);
        return;
    }
    else{
        // Save stabilization data
        tracker.SaveTrackedData();
        // tells to UI that the processing finished
        controller.SetFinished(true);
    }

}

// Apply stabilization to clip
void ClipProcessingJobs::stabilizeClip(Clip& clip, ProcessingController& controller){
	// create CVStabilization object
	CVStabilization stabilizer(processInfoJson, controller); 
    // Start stabilization process
    stabilizer.stabilizeClip(clip);

    // Thread controller. If effect processing is done, save data
    // Else, kill thread
    if(controller.ShouldStop()){
        controller.SetFinished(true);
        return;
    }
    else{
        // Save stabilization data
        stabilizer.SaveStabilizedData();
        // tells to UI that the processing finished
        controller.SetFinished(true);
    }
}

int ClipProcessingJobs::GetProgress(){

    return (int)processingController.GetProgress();
}

bool ClipProcessingJobs::IsDone(){

    if(processingController.GetFinished()){
        t.join();
    }
    return processingController.GetFinished();
}

void ClipProcessingJobs::CancelProcessing(){
    processingController.CancelProcessing();
}