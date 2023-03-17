//
//  restore.m
//  Mousecape
//
//  Created by Alex Zielenski on 2/1/14.
//  Copyright (c) 2014 Alex Zielenski. All rights reserved.
//

#import "shell/browser/mac/mousecloak/restore.h"

#import "shell/browser/mac/mousecloak/CGSInternal/CGSConnection.h"
#import "shell/browser/mac/mousecloak/apply.h"
#import "shell/browser/mac/mousecloak/backup.h"
#import "shell/browser/mac/mousecloak/MCDefs.h"
#import "shell/browser/mac/mousecloak/MCPrefs.h"

#include "base/logging.h"
#include "base/strings/sys_string_conversions.h"

NSString *restoreStringForIdentifier(NSString *identifier) {
    return [identifier substringFromIndex:28];
}

void restoreCursorForIdentifier(NSString *ident) {
    bool registered = false;
    MCIsCursorRegistered(CGSMainConnectionID(), (char *)ident.UTF8String, &registered);

    NSString *restoreIdent = restoreStringForIdentifier(ident);
    NSDictionary *cape = capeWithIdentifier(ident);
    
    MMLog("Restoring cursor %s from %s", restoreIdent.UTF8String, ident.UTF8String);
    if (cape && registered) {
        applyCapeForIdentifier(cape, restoreIdent, YES);
    }

    CGSRemoveRegisteredCursor(CGSMainConnectionID(), (char *)ident.UTF8String, false);
}

void resetAllCursors() {
    MMLog("%%resetAllCursors");
    LOG(ERROR) << "@@resetAllCursors";

    // Backup main cursors first
    NSUInteger i = 0;
    NSString *key = nil;
    while ((key = defaultCursors[i]) != nil) {
        restoreCursorForIdentifier(backupStringForIdentifier(key));
        i++;
    }

    // Backup auxiliary cursors
    MMLog("Restoring core cursors...");
    if (CoreCursorUnregisterAll(CGSMainConnectionID()) == 0) {
        MCSetDefault(NULL, MCPreferencesAppliedCursorKey);
        
        for (int x = 0; x < 45; x++) {
            CoreCursorSet(CGSMainConnectionID(), x);
        }
        
        MMLog(BOLD GREEN "Successfully restored all cursors." RESET);
    } else
        MMLog(BOLD RED "Received an error while restoring core cursors." RESET);
}
