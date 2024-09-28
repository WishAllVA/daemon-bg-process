#include<stdio.h>
#include<stdlib.h>
#include<CoreServices/CoreServices.h>

void fileChangeCallback(
  ConstFSEventStreamRef streamRef,
  void *userData,
  size_t numEvents,
  void *eventPaths,
  const FSEventStreamEventFlags eventFlags[],
  const FSEventStreamEventId eventIds[]
) {
  char **paths = (char **)eventPaths;
  for (size_t i = 0; i < numEvents; i++) {
    printf("File changed: %s", paths[i]);
    char command[512];
    snprintf(command, sizeof(command), "osascript -e 'display notification \"File changed: %s\" with title \"Directory Watcher\"'", paths[i]);
    system(command);
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage %s: <directory_to_watch>", argv[0]);
  }
  const char *dirToWatch = argv[1];
  CFStringRef dirToWatchCF = CFStringCreateWithCString(NULL, dirToWatch, kCFStringEncodingUTF8);
  CFArrayRef pathsToWatch = CFArrayCreate(NULL, (const void **)&dirToWatchCF, 1, NULL);

  FSEventStreamRef stream = FSEventStreamCreate(
    NULL,
    &fileChangeCallback,
    NULL,
    pathsToWatch,
    kFSEventStreamEventIdSinceNow,
    1.0,
    kFSEventStreamCreateFlagFileEvents
  );

  FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
  FSEventStreamStart(stream);

  CFRunLoopRun();

  CFRelease(dirToWatchCF);
  CFRelease(pathsToWatch);
  FSEventStreamStop(stream);
  FSEventStreamInvalidate(stream);
  FSEventStreamRelease(stream);

  return 0;
}
