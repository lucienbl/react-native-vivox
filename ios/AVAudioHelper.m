#import "AVAudioHelper.h"
#import <AVFoundation/AVAudioSession.h>

@implementation AVAudioHelper

- (void) setSpeakerPhone:(bool)value {
    // Get your app's audioSession singleton object
    AVAudioSession *session = [AVAudioSession sharedInstance];

    // Error handling
    BOOL success;
    NSError *error;

    // set the audioSession category.
    // Needs to be Record or PlayAndRecord to use audioRouteOverride:

    success = [session setCategory:AVAudioSessionCategoryPlayAndRecord
                             error:&error];

    if (!success) {
        NSLog(@"AVAudioSession error setting category:%@", error);
    }

    // Set the audioSession override
    success = [session overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker
                                         error:&error];
    if (!success) {
        NSLog(@"AVAudioSession error overrideOutputAudioPort:%@", error);
    }

    // Activate the audio session
    success = [session setActive:YES error:&error];
    if (!success) {
        NSLog(@"AVAudioSession error activating: %@", error);
    } else {
        NSLog(@"AudioSession active");
    }
}

@end
