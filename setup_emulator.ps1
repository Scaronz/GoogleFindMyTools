# Automated Android CLI Emulator Setup for Google Find My Device
# This creates a lightweight virtual Android phone on your PC.

$ErrorActionPreference = "Stop"

# 1. Setup Java Environment
$javaPath = "C:\Program Files\Java\jdk-21.0.11"
if (Test-Path $javaPath) {
    $env:JAVA_HOME = $javaPath
}
Write-Host "[1/6] Using Java: $env:JAVA_HOME" -ForegroundColor Cyan

# 2. Setup Directories
$sdkRoot = "C:\Android"
$cmdlineDir = "$sdkRoot\cmdline-tools\latest"
New-Item -ItemType Directory -Force -Path "$sdkRoot\cmdline-tools" | Out-Null

# 3. Download Android Command Line Tools
$zipUrl = "https://dl.google.com/android/repository/commandlinetools-win-11076708_latest.zip"
$zipFile = "$sdkRoot\cmdline-tools.zip"

if (-not (Test-Path "$cmdlineDir\bin\sdkmanager.bat")) {
    Write-Host "[2/6] Downloading Android Command Line Tools (~130 MB)..." -ForegroundColor Cyan
    Invoke-WebRequest -Uri $zipUrl -OutFile $zipFile
    
    Write-Host "[3/6] Extracting tools..." -ForegroundColor Cyan
    Expand-Archive -Path $zipFile -DestinationPath "$sdkRoot\temp_cmdline" -Force
    
    if (Test-Path $cmdlineDir) { Remove-Item -Recurse -Force $cmdlineDir }
    Move-Item "$sdkRoot\temp_cmdline\cmdline-tools" $cmdlineDir
    Remove-Item -Recurse -Force "$sdkRoot\temp_cmdline"
    Remove-Item -Force $zipFile
} else {
    Write-Host "[2/6] Command Line Tools already downloaded." -ForegroundColor Green
}

# 4. Set PATH
$env:ANDROID_HOME = $sdkRoot
$env:PATH = "$cmdlineDir\bin;$sdkRoot\emulator;$sdkRoot\platform-tools;$env:PATH"

# 5. Accept Licenses & Install Emulator + Google Play System Image
Write-Host "[4/6] Installing Emulator and Google Play System Image (~1.4 GB)..." -ForegroundColor Cyan
$sdkManager = "$cmdlineDir\bin\sdkmanager.bat"

# Auto-accept licenses
$process = Start-Process -FilePath $sdkManager -ArgumentList "--sdk_root=$sdkRoot", "--licenses" -RedirectStandardInput ([System.IO.Path]::GetTempFileName()) -NoNewWindow -PassThru
# Send 'y' for all prompts
Start-Sleep -Seconds 2
& cmd.exe /c "echo y| `"$sdkManager`" --sdk_root=`"$sdkRoot`" --licenses >nul 2>&1"

# Download emulator, platform-tools, and Google Play system image (Android 14 / API 34)
& $sdkManager --sdk_root="$sdkRoot" "platform-tools" "emulator" "system-images;android-34;google_apis_playstore;x86_64"

# 6. Create AVD if it doesn't exist
$avdName = "GoogleTracker"
$avdManager = "$cmdlineDir\bin\avdmanager.bat"
Write-Host "[5/6] Creating Android Virtual Device: $avdName..." -ForegroundColor Cyan
& cmd.exe /c "echo no | `"$avdManager`" create avd -n $avdName -k `"system-images;android-34;google_apis_playstore;x86_64`" --device `"pixel_6`" --force"

# 7. Start the Emulator
Write-Host "[6/6] Launching Android Emulator..." -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Yellow
Write-Host "Once the phone starts up on your screen:" -ForegroundColor Yellow
Write-Host "1. Go to Settings > Security > Screen lock > Set a PIN (e.g. 1234)" -ForegroundColor Yellow
Write-Host "2. Open Google Play Store > Sign in with your Google account" -ForegroundColor Yellow
Write-Host "3. Go to Settings > Google > All Services > Find My Device" -ForegroundColor Yellow
Write-Host "   Set 'Find your offline devices' to 'With network in all areas'" -ForegroundColor Yellow
Write-Host "============================================================" -ForegroundColor Yellow

$emulatorExe = "$sdkRoot\emulator\emulator.exe"
Start-Process -FilePath $emulatorExe -ArgumentList "-avd", $avdName
