# Descobre o IP deste computador na rede Wi-Fi atual e grava no .env da web (VITE_BROKER_HOST)
# para preencher automaticamente o campo de IP do servidor no pareamento/reconfiguracao Bluetooth.
# O secrets.h do firmware permanece limpo (operacao 100% via BLE/NVS).
$ErrorActionPreference = "Continue"
$ips = @()
try {
    $ips = Get-NetIPAddress -AddressFamily IPv4 |
        Where-Object { $_.IPAddress -notmatch "^(127\.|169\.254\.)" -and $_.InterfaceAlias -notmatch "vEthernet|WSL|Loopback|VirtualBox|VMware|Docker|Bluetooth" } |
        Select-Object -ExpandProperty IPAddress
} catch { }
if ($ips.Count -eq 0) { Write-Host "Nao consegui descobrir o IP. Rode ipconfig." -ForegroundColor Red; exit 1 }
$ip = $ips | Select-Object -First 1
Write-Host "IP deste computador na rede atual: $ip" -ForegroundColor Green
$ips | ForEach-Object { Write-Host "  encontrado: $_" }

$aqui = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Content -Path (Join-Path $aqui "meu-ip.txt") -Value ($ips -join "`n") -Encoding ASCII

# Atualiza o .env do Portal Web para ja sugerir este IP na tela de pareamento/reconfiguracao Bluetooth
$webEnv = Join-Path $aqui "cronomed-web\.env"
if (Test-Path $webEnv) {
    $envTxt = [System.IO.File]::ReadAllText($webEnv)
    if ($envTxt -match "(?m)^VITE_BROKER_HOST=") {
        $envTxt = [regex]::Replace($envTxt, "(?m)^VITE_BROKER_HOST=.*", "VITE_BROKER_HOST=$ip")
    } else {
        $envTxt = $envTxt.TrimEnd() + "`r`nVITE_BROKER_HOST=$ip`r`n"
    }
    [System.IO.File]::WriteAllText($webEnv, $envTxt, (New-Object System.Text.UTF8Encoding($false)))
} else {
    [System.IO.File]::WriteAllText($webEnv, "VITE_BROKER_HOST=$ip`r`n", (New-Object System.Text.UTF8Encoding($false)))
}
Write-Host "cronomed-web\.env atualizado: VITE_BROKER_HOST=$ip (sugerido automaticamente no pareamento Bluetooth)" -ForegroundColor Cyan