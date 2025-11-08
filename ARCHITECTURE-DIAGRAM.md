# Blue-Green Module - Complete Architecture Diagram

Visual representation of all functions across all files and their relationships.

## Complete System Architecture

```mermaid
graph TB
    %% Styling
    classDef moduleFunc fill:#e1f5ff,stroke:#0066cc,stroke-width:2px
    classDef adminFunc fill:#fff4e1,stroke:#ff9900,stroke-width:2px
    classDef installFunc fill:#ffe1e1,stroke:#cc0000,stroke-width:2px
    classDef batchFunc fill:#e8f5e9,stroke:#4caf50,stroke-width:3px

    subgraph "bluegreen.module (Core)"
        %% Environment Functions
        get_active[get_active_environment]:::moduleFunc
        get_env_info[get_environment_info]:::moduleFunc
        is_configured[is_configured]:::moduleFunc
        get_idle[get_idle_environment]:::moduleFunc
        get_shared[get_shared_tables]:::moduleFunc
        get_table_count[get_table_count]:::moduleFunc
        env_installed[environment_is_installed]:::moduleFunc

        %% Config Functions
        detect_config[detect_config_directory]:::moduleFunc
        get_config_dir[get_config_directory]:::moduleFunc
        count_config[count_config_files]:::moduleFunc
        copy_dir[copy_directory]:::moduleFunc
        sync_config[sync_config_files]:::moduleFunc
        create_alt[create_alt_config_directory]:::moduleFunc

        %% State Functions
        get_last_sync[get_last_sync]:::moduleFunc
        set_last_sync[set_last_sync]:::moduleFunc
    end

    subgraph "bluegreen.admin.inc (Admin UI)"
        %% Forms
        admin_form[admin_form]:::adminFunc
        setup_form[setup_form]:::adminFunc

        %% Submit Handlers
        backup_submit[backup_submit]:::adminFunc
        sync_submit[sync_submit]:::adminFunc
        switch_submit[switch_submit]:::adminFunc
        setup_submit[setup_form_submit]:::adminFunc

        %% Callbacks
        switch_callback[switch_environment_callback]:::adminFunc
        sync_callback[sync_environment_callback]:::adminFunc
        backup_callback[backup_environment_callback]:::adminFunc
        clear_cache_callback[clear_cache_callback]:::adminFunc

        %% Core Operations
        sync_database[sync_database]:::adminFunc
        copy_table[copy_table]:::adminFunc
        switch_environment[switch_environment]:::adminFunc

        %% Batch Operations
        sync_batch_op[sync_batch_operation]:::batchFunc
        sync_batch_finish[sync_batch_finished]:::batchFunc
        setup_dup_tables[setup_duplicate_tables]:::batchFunc
        setup_create_config[setup_create_config_directories]:::batchFunc
        setup_write_settings[setup_write_settings]:::batchFunc
        setup_finalize[setup_finalize]:::batchFunc
        setup_finished[setup_finished]:::batchFunc
    end

    subgraph "bluegreen.install (Install/Uninstall)"
        requirements[requirements]:::installFunc
        install[install]:::installFunc
        uninstall[uninstall]:::installFunc
    end

    %% Cross-file relationships - Admin to Module
    admin_form --> is_configured
    admin_form --> get_active
    admin_form --> get_idle
    admin_form --> get_env_info
    admin_form --> env_installed
    admin_form --> get_last_sync

    setup_form --> is_configured

    switch_submit --> get_active
    switch_submit --> get_idle
    switch_submit --> switch_environment

    sync_batch_op --> sync_database
    sync_batch_op --> sync_config
    sync_batch_op --> set_last_sync

    sync_callback --> sync_database
    sync_callback --> sync_config
    sync_callback --> set_last_sync

    switch_callback --> switch_environment

    setup_dup_tables --> get_shared
    setup_create_config --> create_alt
    setup_create_config --> count_config

    %% Within-module relationships
    get_idle --> get_active
    is_configured --> get_env_info
    is_configured --> env_installed
    env_installed --> get_table_count
    get_table_count --> get_env_info

    get_config_dir --> detect_config
    count_config --> get_config_dir
    sync_config --> get_config_dir
    sync_config --> copy_dir
    create_alt --> detect_config
    create_alt --> copy_dir
    copy_dir -.recursive.-> copy_dir

    %% Within-admin relationships
    sync_database --> get_env_info
    sync_database --> get_shared
    switch_environment --> get_env_info

    sync_submit --> sync_batch_op
    setup_submit --> setup_dup_tables
    setup_submit --> setup_create_config
    setup_submit --> setup_write_settings
    setup_submit --> setup_finalize
```

## File-by-File Breakdown

### bluegreen.module (18 functions)

```mermaid
graph LR
    subgraph "Environment Management"
        A1[get_active_environment]
        A2[get_idle_environment]
        A3[get_environment_info]
        A4[is_configured]
        A5[environment_is_installed]
        A6[get_table_count]
        A7[get_shared_tables]

        A2 --> A1
        A4 --> A3
        A4 --> A5
        A5 --> A6
        A6 --> A3
    end

    subgraph "Config Management"
        B1[detect_config_directory]
        B2[get_config_directory]
        B3[count_config_files]
        B4[copy_directory]
        B5[sync_config_files]
        B6[create_alt_config_directory]

        B2 --> B1
        B3 --> B2
        B5 --> B2
        B5 --> B4
        B6 --> B1
        B6 --> B4
        B4 -.recursive.-> B4
    end

    subgraph "State Management"
        C1[get_last_sync]
        C2[set_last_sync]
    end

    style A1 fill:#e1f5ff
    style A2 fill:#e1f5ff
    style A3 fill:#e1f5ff
    style A4 fill:#e1f5ff
    style A5 fill:#e1f5ff
    style A6 fill:#e1f5ff
    style A7 fill:#e1f5ff
    style B1 fill:#c8e6c9
    style B2 fill:#c8e6c9
    style B3 fill:#c8e6c9
    style B4 fill:#c8e6c9
    style B5 fill:#c8e6c9
    style B6 fill:#c8e6c9
    style C1 fill:#fff9c4
    style C2 fill:#fff9c4
```

### bluegreen.admin.inc (20 functions)

```mermaid
graph TD
    subgraph "User Interface"
        U1[admin_form]
        U2[setup_form]
    end

    subgraph "Submit Handlers"
        S1[backup_submit]
        S2[sync_submit]
        S3[switch_submit]
        S4[setup_form_submit]
    end

    subgraph "Menu Callbacks"
        C1[switch_environment_callback]
        C2[sync_environment_callback]
        C3[backup_environment_callback]
        C4[clear_cache_callback]
    end

    subgraph "Core Operations"
        O1[switch_environment]
        O2[sync_database]
        O3[copy_table]
    end

    subgraph "Batch System"
        B1[sync_batch_operation]
        B2[sync_batch_finished]
        B3[setup_duplicate_tables]
        B4[setup_create_config_directories]
        B5[setup_write_settings]
        B6[setup_finalize]
        B7[setup_finished]
    end

    %% Relationships
    S2 --> B1
    S3 --> O1
    S4 --> B3
    S4 --> B4
    S4 --> B5
    S4 --> B6

    C1 --> O1
    C2 --> O2

    B1 --> O2
    B1 --> B2

    style U1 fill:#e1f5ff
    style U2 fill:#e1f5ff
    style S1 fill:#fff4e1
    style S2 fill:#fff4e1
    style S3 fill:#fff4e1
    style S4 fill:#fff4e1
    style O1 fill:#ffccbc
    style O2 fill:#ffccbc
    style O3 fill:#ffccbc
    style B1 fill:#c8e6c9
    style B2 fill:#c8e6c9
    style B3 fill:#c8e6c9
    style B4 fill:#c8e6c9
    style B5 fill:#c8e6c9
    style B6 fill:#c8e6c9
    style B7 fill:#c8e6c9
```

## User Action Flow Diagrams

### Environment Switch Flow

```mermaid
sequenceDiagram
    participant User
    participant UI as admin_form
    participant Submit as switch_submit
    participant Core as switch_environment
    participant Cache as clear_cache_callback
    participant Module as bluegreen.module

    User->>UI: Click "Make Green Live"
    UI->>Submit: Form submission
    Submit->>Module: get_active_environment()
    Submit->>Module: get_idle_environment()
    Submit->>Core: switch_environment('green')
    Core->>Module: get_environment_info('green')
    Core->>Core: Update settings.bluegreen.php
    Core->>Core: state_set('bluegreen_active_environment', 'green')
    Submit->>Cache: Redirect to clear_cache
    Cache->>Cache: backdrop_flush_all_caches()
    Cache->>UI: Redirect to admin page
    UI->>User: Show success message
```

### Database Sync Flow

```mermaid
sequenceDiagram
    participant User
    participant UI as admin_form
    participant Submit as sync_submit
    participant Batch as sync_batch_operation
    participant DB as sync_database
    participant Config as sync_config_files
    participant Module as bluegreen.module

    User->>UI: Click "Sync from Blue"
    UI->>Submit: Form submission
    Submit->>Batch: batch_set()
    Batch->>Module: sync_database('blue', 'green')
    DB->>Module: get_environment_info()
    DB->>Module: get_shared_tables()
    DB->>DB: DROP target tables
    DB->>DB: CREATE TABLE LIKE source
    DB->>DB: INSERT INTO target SELECT * FROM source
    Batch->>Module: sync_config_files('blue', 'green')
    Config->>Module: get_config_directory()
    Config->>Module: copy_directory()
    Batch->>Module: set_last_sync('green')
    Batch->>UI: Show completion message
```

### Initial Setup Flow

```mermaid
sequenceDiagram
    participant User
    participant Setup as setup_form
    participant Submit as setup_form_submit
    participant B1 as setup_duplicate_tables
    participant B2 as setup_create_config_directories
    participant B3 as setup_write_settings
    participant B4 as setup_finalize
    participant Module as bluegreen.module

    User->>Setup: Fill setup form
    User->>Setup: Choose initial active env
    Setup->>Submit: Submit form
    Submit->>B1: Batch: Duplicate tables
    B1->>Module: get_shared_tables()
    B1->>B1: CREATE TABLE alt_* LIKE original
    B1->>B1: INSERT INTO alt_* SELECT * FROM original
    Submit->>B2: Batch: Create config dirs
    B2->>Module: create_alt_config_directory()
    Module->>Module: detect_config_directory()
    Module->>Module: copy_directory()
    Submit->>B3: Batch: Write settings
    B3->>B3: Create settings.bluegreen.php
    B3->>B3: Update settings.php
    Submit->>B4: Batch: Finalize
    B4->>B4: state_set('bluegreen_active_environment')
    B4->>B4: backdrop_flush_all_caches()
    B4->>User: Show success messages
```

## Function Complexity Analysis

### High Complexity (5+ function calls)
- **admin_form()** - Calls 7 module functions
- **sync_batch_operation()** - Calls 3 functions + manages batch state
- **setup_create_config_directories()** - Calls 5 functions

### Medium Complexity (2-4 function calls)
- **is_configured()** - Calls 2 functions
- **sync_config_files()** - Calls 2 functions
- **create_alt_config_directory()** - Calls 2 functions
- **switch_submit()** - Calls 3 functions
- **sync_database()** - Calls 2 functions + DB operations

### Low Complexity (0-1 function calls)
- Most getter functions (get_active_environment, get_shared_tables, etc.)
- State management functions
- Simple callbacks

## Critical Paths

### Most Important Functions (Called by Many)
1. **get_environment_info()** - Used throughout system for DB config
2. **get_active_environment()** - Used by UI and operations
3. **is_configured()** - Gate-keeper for functionality
4. **get_shared_tables()** - Critical for data integrity during sync

### Risk Areas (Single Point of Failure)
1. **switch_environment()** - File write operations, state changes
2. **sync_database()** - Large data operations, DROP/CREATE tables
3. **setup_duplicate_tables()** - Initial setup, creates all alt_ tables

## Legend

- 🔵 **Blue** - bluegreen.module functions
- 🟠 **Orange** - bluegreen.admin.inc functions
- 🔴 **Red** - bluegreen.install functions
- 🟢 **Green** - Batch operation functions
- **Solid lines** - Direct function calls
- **Dotted lines** - Recursive calls

---

**Generated:** 2025-11-08
**Total Functions Mapped:** 47 across 3 files
