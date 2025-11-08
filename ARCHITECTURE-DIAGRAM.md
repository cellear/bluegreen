# Blue-Green Module - Complete Architecture Diagram

Visual representation of all functions across all files and their relationships.

## Complete System Overview

```mermaid
graph TD
    subgraph "USER INTERFACE LAYER"
        admin_form[admin_form<br/>Main UI]:::adminFunc
        setup_form[setup_form<br/>Setup Wizard]:::adminFunc
    end

    subgraph "SUBMIT HANDLERS"
        backup_submit[backup_submit]:::adminFunc
        sync_submit[sync_submit]:::adminFunc
        switch_submit[switch_submit]:::adminFunc
        setup_submit[setup_form_submit]:::adminFunc
    end

    subgraph "CORE OPERATIONS"
        switch_env[switch_environment]:::adminFunc
        sync_db[sync_database]:::adminFunc
    end

    subgraph "BATCH OPERATIONS"
        sync_batch_op[sync_batch_operation]:::batchFunc
        setup_dup[setup_duplicate_tables]:::batchFunc
        setup_config[setup_create_config_directories]:::batchFunc
        setup_settings[setup_write_settings]:::batchFunc
        setup_final[setup_finalize]:::batchFunc
    end

    subgraph "MODULE FUNCTIONS"
        direction TB
        subgraph "Environment"
            get_active[get_active_environment]:::moduleFunc
            get_env_info[get_environment_info]:::moduleFunc
            is_configured[is_configured]:::moduleFunc
            env_installed[environment_is_installed]:::moduleFunc
            get_shared[get_shared_tables]:::moduleFunc
        end

        subgraph "Configuration"
            detect_config[detect_config_directory]:::moduleFunc
            get_config_dir[get_config_directory]:::moduleFunc
            sync_config[sync_config_files]:::moduleFunc
            create_alt[create_alt_config_directory]:::moduleFunc
        end

        subgraph "State"
            set_last_sync[set_last_sync]:::moduleFunc
        end
    end

    %% UI to Handlers
    admin_form --> switch_submit
    admin_form --> sync_submit
    setup_form --> setup_submit

    %% Handlers to Operations
    switch_submit --> switch_env
    sync_submit --> sync_batch_op
    setup_submit --> setup_dup
    setup_submit --> setup_config
    setup_submit --> setup_settings
    setup_submit --> setup_final

    %% Operations to Module
    sync_batch_op --> sync_db
    sync_batch_op --> sync_config
    sync_batch_op --> set_last_sync

    switch_env --> get_env_info
    sync_db --> get_env_info
    sync_db --> get_shared

    admin_form --> is_configured
    admin_form --> get_active

    setup_dup --> get_shared
    setup_config --> create_alt

    is_configured --> env_installed
    sync_config --> get_config_dir
    get_config_dir --> detect_config

    classDef moduleFunc fill:#e1f5ff,stroke:#0066cc,stroke-width:2px
    classDef adminFunc fill:#fff4e1,stroke:#ff9900,stroke-width:2px
    classDef batchFunc fill:#e8f5e9,stroke:#4caf50,stroke-width:3px
```

## Environment Switch Flow

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant UI as admin_form
    participant Handler as switch_submit
    participant Core as switch_environment
    participant Module as module functions

    User->>UI: Click "Make Green Live"
    UI->>Handler: Submit form
    Handler->>Module: get_active_environment()
    Module-->>Handler: 'blue'
    Handler->>Module: get_idle_environment()
    Module-->>Handler: 'green'
    Handler->>Core: switch_environment('green')
    Core->>Module: get_environment_info('green')
    Module-->>Core: DB config
    Core->>Core: Update settings.bluegreen.php
    Core->>Core: state_set('bluegreen_active_environment')
    Core-->>Handler: Success
    Handler->>Handler: backdrop_flush_all_caches()
    Handler->>UI: Redirect
    UI->>User: "Environment switched!"
```

## Database Sync Flow

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant UI as admin_form
    participant Handler as sync_submit
    participant Batch as sync_batch_operation
    participant DB as sync_database
    participant Config as sync_config_files
    participant Module as module functions

    User->>UI: Click "Sync from Blue"
    UI->>Handler: Submit form
    Handler->>Batch: batch_set()
    activate Batch
    Batch->>DB: sync_database('blue', 'green')
    activate DB
    DB->>Module: get_environment_info()
    DB->>Module: get_shared_tables()
    DB->>DB: DROP target tables
    DB->>DB: CREATE TABLE LIKE
    DB->>DB: INSERT INTO SELECT
    deactivate DB
    Batch->>Config: sync_config_files('blue', 'green')
    activate Config
    Config->>Module: get_config_directory()
    Config->>Module: copy_directory()
    deactivate Config
    Batch->>Module: set_last_sync('green')
    deactivate Batch
    Batch->>UI: Show results
    UI->>User: "Synced N tables!"
```

## Initial Setup Flow

```mermaid
sequenceDiagram
    autonumber
    participant User
    participant Setup as setup_form
    participant Submit as setup_form_submit
    participant B1 as Batch: Duplicate Tables
    participant B2 as Batch: Create Config
    participant B3 as Batch: Write Settings
    participant B4 as Batch: Finalize

    User->>Setup: Choose active environment
    Setup->>Submit: Submit
    Submit->>B1: Start batch
    activate B1
    Note over B1: CREATE TABLE alt_*<br/>INSERT INTO alt_*
    deactivate B1
    Submit->>B2: Next batch step
    activate B2
    Note over B2: create_alt_config_directory()<br/>copy all .json files
    deactivate B2
    Submit->>B3: Next batch step
    activate B3
    Note over B3: Create settings.bluegreen.php<br/>Update settings.php
    deactivate B3
    Submit->>B4: Final batch step
    activate B4
    Note over B4: state_set()<br/>backdrop_flush_all_caches()
    deactivate B4
    B4->>User: "Setup complete!"
```

## Module Function Groups

### bluegreen.module (18 functions)

```mermaid
graph TD
    subgraph "Hooks"
        H1[menu :13]
        H2[permission :84]
        H3[init :549]
        H4[admin_bar_output_build :559]
    end

    subgraph "Environment Management"
        E1[get_active_environment :110]
        E2[get_idle_environment :160]
        E3[get_environment_info :124]
        E4[is_configured :140]
        E5[environment_is_installed :289]
        E6[get_table_count :224]
        E7[get_shared_tables :174]
    end

    subgraph "Configuration Management"
        C1[detect_config_directory :302]
        C2[get_config_directory :333]
        C3[count_config_files :362]
        C4[copy_directory :386]
        C5[sync_config_files :439]
        C6[create_alt_config_directory :489]
    end

    subgraph "State Management"
        S1[get_last_sync :196]
        S2[set_last_sync :208]
    end

    %% Hook dependencies
    H4 --> E4
    H4 --> E1

    %% Environment dependencies
    E2 --> E1
    E4 --> E3
    E4 --> E5
    E5 --> E6
    E6 --> E3

    %% Config dependencies
    C2 --> C1
    C3 --> C2
    C5 --> C2
    C5 --> C4
    C6 --> C1
    C6 --> C4
    C4 -.recursive.-> C4

    style H1 fill:#e1f5ff
    style H2 fill:#e1f5ff
    style H3 fill:#e1f5ff
    style H4 fill:#e1f5ff
    style E1 fill:#fff4e1
    style E2 fill:#fff4e1
    style E3 fill:#fff4e1
    style E4 fill:#fff4e1
    style E5 fill:#fff4e1
    style E6 fill:#fff4e1
    style E7 fill:#fff4e1
    style C1 fill:#e8f5e9
    style C2 fill:#e8f5e9
    style C3 fill:#e8f5e9
    style C4 fill:#e8f5e9
    style C5 fill:#e8f5e9
    style C6 fill:#e8f5e9
    style S1 fill:#f3e5f5
    style S2 fill:#f3e5f5
```

### bluegreen.admin.inc (20 functions)

```mermaid
graph TD
    subgraph "Forms"
        F1[admin_form :10]
        F2[setup_form :676]
    end

    subgraph "Submit Handlers"
        SH1[backup_submit :248]
        SH2[sync_submit :274]
        SH3[switch_submit :354]
        SH4[setup_form_submit :763]
    end

    subgraph "Menu Callbacks"
        CB1[switch_environment_callback :587]
        CB2[sync_environment_callback :603]
        CB3[backup_environment_callback :636]
        CB4[clear_cache_callback :659]
    end

    subgraph "Core Operations"
        OP1[switch_environment :506]
        OP2[sync_database :387]
        OP3[copy_table :445]
    end

    subgraph "Batch Operations"
        B1[sync_batch_operation :295]
        B2[sync_batch_finished :329]
        B3[setup_duplicate_tables :785]
        B4[setup_create_config_directories :859]
        B5[setup_write_settings :889]
        B6[setup_finalize :1070]
        B7[setup_finished :1084]
    end

    %% Submit to Operations
    SH2 --> B1
    SH3 --> OP1
    SH4 --> B3
    SH4 --> B4
    SH4 --> B5
    SH4 --> B6

    %% Callbacks to Operations
    CB1 --> OP1
    CB2 --> OP2

    %% Batch relationships
    B1 --> OP2
    B1 --> B2

    style F1 fill:#e1f5ff
    style F2 fill:#e1f5ff
    style SH1 fill:#fff4e1
    style SH2 fill:#fff4e1
    style SH3 fill:#fff4e1
    style SH4 fill:#fff4e1
    style CB1 fill:#ffccbc
    style CB2 fill:#ffccbc
    style CB3 fill:#ffccbc
    style CB4 fill:#ffccbc
    style OP1 fill:#ffe0b2
    style OP2 fill:#ffe0b2
    style OP3 fill:#ffe0b2
    style B1 fill:#c8e6c9
    style B2 fill:#c8e6c9
    style B3 fill:#c8e6c9
    style B4 fill:#c8e6c9
    style B5 fill:#c8e6c9
    style B6 fill:#c8e6c9
    style B7 fill:#c8e6c9
```

### bluegreen.install (3 functions)

```mermaid
graph TD
    I1[requirements :10<br/>Check configuration status]
    I2[install :52<br/>Display welcome message]
    I3[uninstall :60<br/>Clean up alt_ tables and settings]

    style I1 fill:#ffcdd2
    style I2 fill:#ffcdd2
    style I3 fill:#ffcdd2
```

## Function Complexity Analysis

### High Complexity (5+ calls)
```mermaid
graph TD
    A[admin_form]
    A --> B[is_configured]
    A --> C[get_active]
    A --> D[get_idle]
    A --> E[get_env_info]
    A --> F[env_installed]
    A --> G[get_last_sync]
    A --> H[module_exists]

    style A fill:#ffccbc,stroke:#ff5722,stroke-width:3px
```

### Medium Complexity (2-4 calls)
- **sync_batch_operation** → sync_database, sync_config_files, set_last_sync
- **is_configured** → get_environment_info, environment_is_installed
- **switch_submit** → get_active, get_idle, switch_environment

### Low Complexity (0-1 calls)
- Getter functions (get_active_environment, get_shared_tables, etc.)
- State functions (get_last_sync, set_last_sync)
- Simple callbacks

## Critical Paths

### Most Called Functions
```mermaid
graph TD
    A[get_environment_info<br/>Called by 3+ functions]
    B[get_active_environment<br/>Called by multiple UI functions]
    C[is_configured<br/>Gate-keeper function]
    D[get_shared_tables<br/>Critical for data integrity]

    style A fill:#ff5722,color:#fff
    style B fill:#ff9800,color:#fff
    style C fill:#ffc107
    style D fill:#ffc107
```

### Risk Areas (Single Point of Failure)
```mermaid
graph TD
    R1[switch_environment<br/>File write operations]
    R2[sync_database<br/>Large data operations]
    R3[setup_duplicate_tables<br/>Initial table creation]

    style R1 fill:#f44336,color:#fff,stroke:#b71c1c,stroke-width:3px
    style R2 fill:#f44336,color:#fff,stroke:#b71c1c,stroke-width:3px
    style R3 fill:#f44336,color:#fff,stroke:#b71c1c,stroke-width:3px
```

## Legend

- 🔵 **Light Blue** - bluegreen.module functions
- 🟠 **Orange** - bluegreen.admin.inc functions (forms, handlers)
- 🟢 **Green** - Batch operation functions
- 🔴 **Red** - bluegreen.install functions
- **Solid lines** - Direct function calls
- **Dotted lines** - Recursive calls
- **Thick borders** - High-risk or high-complexity functions

## Summary Statistics

| Metric | Count |
|--------|-------|
| **Total Functions** | 47 |
| **bluegreen.module** | 18 |
| **bluegreen.admin.inc** | 20 |
| **bluegreen.install** | 3 |
| **Sample modules** | 6 |
| **Hook Implementations** | 10 |
| **Menu Callbacks** | 7 |
| **Batch Operations** | 5 |
| **Cross-file Dependencies** | 15+ |

---

**Generated:** 2025-11-08
**Total Functions Mapped:** 47 across 3 core files
**Diagram Format:** Mermaid (renders on GitHub)
