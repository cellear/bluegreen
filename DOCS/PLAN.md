# Blue-Green Deployment Module Enhancement Plan

## Overview
Enhance the Blue-Green Deployment module with UI improvements, domain routing, dual-database architecture refactoring, and better data management to create a production-ready deployment system.

## Phase 1: Critical Fixes & Demo Prep

### 1. Fix Environment Switching Bug
**Files:** `bluegreen.admin.inc`

**Problem:** The regex pattern in `bluegreen_switch_environment()` (line 436) fails to match the settings.php structure.

**Solution:** Update the regex pattern to handle the actual database configuration format or switch to using Backdrop's `backdrop_rewrite_settings()` API instead of manual regex replacement.

### 2. Color Admin Toolbar by Environment  
**Files:** `bluegreen.module`, `css/bluegreen.admin.css`

**Implementation:**
- Add CSS to color the entire admin toolbar background (not just a badge) blue or green
- Hook into `hook_page_alter()` or `hook_html_head_alter()` to inject inline CSS
- Use the active environment from `bluegreen_get_active_environment()`
- Target Backdrop's `.l-header` or admin bar classes

### 3. Dual Domain Support (DDEV)
**Files:** `.ddev/config.yaml` (create if needed), `settings.php`

**Implementation:**
- Configure DDEV with `additional_fqdns` for dev domain (e.g., `dev.bluegreen-v2.ddev.site`)
- In `settings.php`, add logic to check `$_SERVER['HTTP_HOST']`
- Map prod domain → active environment database
- Map dev domain → idle environment database
- Document DDEV setup in README

## Phase 2: UI Improvements

### 4. Compact Layout
**Files:** `bluegreen.admin.inc`, `css/bluegreen.admin.css`

**Changes:**
- Reduce vertical spacing in environment panels
- Move "Make Live" and "Sync" buttons between the two environment panes or above them
- Make info sections more compact (reduce padding)
- Ensure all controls fit on MacBook screen without scrolling

### 5. Move Menu Location
**Files:** `bluegreen.module`

**Change:** Move from `admin/config/development/bluegreen` to `admin/content/bluegreen`
- Update `bluegreen_menu()` paths
- Update `.info` file configure path
- Add to Content menu dropdown

### 6. Enhanced Sync Feedback
**Files:** `bluegreen.admin.inc`

**Change:** In `bluegreen_sync_batch_finished()` and `bluegreen_sync_database()`:
- Count tables being copied
- Display "Successfully synced X tables from Y to Z"

### 7. Add Switch Comments
**Files:** `bluegreen.admin.inc`, `bluegreen.module`

**Implementation:**
- Add optional textarea to switch form
- Store comment in watchdog or dedicated state variable with timestamp
- Display comment history on overview page

## Phase 3: Architecture Refactor

### 8. Simplify to Two Table Sets
**Files:** `bluegreen.admin.inc`, `bluegreen.install`, `settings.php`

**Major Change:**
- Currently: 3 sets (default, blue_*, green_*)
- New: 2 sets (unprefixed = "Blue", alt_* = "Green")
- Update all setup/sync/switch logic
- Migrate existing installations (if any)

### 9-13. Table Handling Strategy

**Files:** `bluegreen.admin.inc`

Implement table exclusion lists in `bluegreen_sync_database()`:

**9. Shared Watchdog:** Exclude `watchdog` from sync
**10. Shared Users/Roles:** Exclude `users`, `role`, `role_permission`, `authmap` from sync  
**11. Shared Sessions:** Exclude `sessions` from sync
**12. Clear Cache on Switch:** Add cache clear in `bluegreen_switch_environment()` (already exists, but ensure it's comprehensive)
**13. Separate File Storage:** 
- Create separate files directories: `files/` (blue) and `files_alt/` (green)
- Sync `file_managed`, `file_usage` tables
- Copy actual files on disk during sync

### 14. Config File Management
**Files:** `bluegreen.admin.inc`, new helper functions

**Implementation:**
- Leverage `$config_directories['active']` and `['staging']`
- When syncing: copy both database AND config files
- When switching: swap which config directory is "active"
- May need custom config directory paths per environment

## Phase 4: Polish

### 15. Clean Uninstall
**Files:** `bluegreen.install`

**Implementation:**
- In `hook_uninstall()`, prompt or automatically delete `alt_*` tables
- Clean up state variables
- Optional: restore to single-database setup

### 16. Add Tests
**Files:** `bluegreen.test` (new)

**Implementation:**
- Use Backdrop's `.test` file format
- Test environment switching
- Test database sync
- Test permissions
- Review existing Backdrop modules for test patterns

## Implementation Notes

**Priority Order:**
1. Bug fix (#1) - blocks everything
2. Colored toolbar (#2) - visual impact
3. Dual domain (#3) - demonstrates concept
4. Compact UI (#4) - usability
5. All others as needed

**Dependencies:**
- #8 (two table sets) should be done before #9-13 (table handling)
- #14 (config) can be independent but pairs well with #9-13

**Testing Strategy:**
- Test each phase on DDEV before production
- Create database backup before major changes
- Document rollback procedures

## Feature List Summary

### Bug Fixes
1. Environment switching failure - regex pattern error

### Features
1. Dual domain support (DDEV)
2. UI improvements (colored toolbar + menu move)
3. Enhanced sync feedback (table count)
4. Compact UI layout
5. Clean uninstall process
6. Simplify to two table sets
7. Add tests
8. Config file management
9. Shared watchdog logs
10. Shared user/role/permission tables
11. Shared session tables
12. Clear cache on environment switch
13. Separate file storage per environment
14. Add comments for environment switches

