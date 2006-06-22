!===========================================================================
!
!  autobuild.l - iMatix autobuilder
!
!  Written:  2005/09/28  Martin Lucina
!  Revised:  
!
!===========================================================================

-idle
-nosort
-schema:lrschemanew.pl

After-Init:
    (--) Ok                                 -> Determine-Build-State
          + Load-Config-File
          + Initialise-Svn-Client
          + Load-State
          + Check-Build-State
    (--) Config-Error                       ->
          + Terminate-With-Error
    (--) Svn-Error                          -> 
          + Terminate-With-Error

Determine-Build-State:
    (--) Build-First-Time                   -> Build-Products
          + Mark-Products-As-Out-Of-Date    ! To force build of all products
          + Get-New-Build-Id
          + Start-Build
          + Get-Next-Product
    (--) Rebuild                            -> Build-Products
          + Mark-Products-As-Out-Of-Date    ! To force build of all products
          + Get-New-Build-Id
          + Start-Build
          + Get-Next-Product
    (--) Build-Interrupted                  -> Build-Products
          + Start-Build
          + Get-Next-Product
    (--) Build-Succeeded                    -> Wait-For-Svn-Changes
          + Check-If-Svn-Has-Changed        
    (--) Build-Failed                       -> Wait-For-Svn-Changes
          + Check-If-Svn-Has-Changed

Wait-For-Svn-Changes:
    (--) Svn-Has-Not-Changed                -> Wait-For-Svn-Changes
          + Wait-A-While
          + Check-If-Svn-Has-Changed
    (--) Svn-Has-Changed                    -> Wait-For-Svn-Changes
          + Update-Products
    (--) Products-Have-Not-Changed          -> Wait-For-Svn-Changes
          + Wait-A-While
          + Check-If-Svn-Has-Changed
    (--) Products-Have-Changed              -> Build-Products
          + Get-New-Build-Id
          + Start-Build
          + Get-Next-Product

Build-Products:
    (--) Product                            -> Build-Products
          + Prepare-Product
          + Build-Product
          + Install-Product
          + Test-Product
          + Package-Product
          + Upload-Product
          + Get-Next-Product
    (--) Finished                           -> Wait-For-Svn-Changes
          + Mark-Build-As-Succeeded
          + Mark-Products-As-Up-To-Date
          + Save-State
          + Send-Build-Report
          + Wait-A-While
          + Check-If-Svn-Has-Changed
    (--) Build-Error                        -> Wait-For-Svn-Changes
          + Mark-Build-As-Failed
          + Mark-Products-As-Up-To-Date
          + Save-State
          + Send-Build-Report
          + Wait-A-While
          + Check-If-Svn-Has-Changed
    (--) Shutdown                           -> 
          + Mark-Build-As-Interrupted
          + Mark-Products-As-Out-Of-Date    ! To force rebuild next time
          + Save-State
          + Terminate-The-Program

Defaults:
    (--) Exception                          ->
          + Terminate-With-Error
    (--) Svn-Error                          -> Wait-For-Svn-Changes
          + Wait-A-While
          + Check-If-Svn-Has-Changed
    (--) Shutdown                           -> 
          + Terminate-The-Program
