#!/data/data/com.termux/files/usr/bin/bash
# SIPV V1.0 - OMNI DEPLOYMENT ENGINE (FORCE-FIXED)

echo "[*] Re-Initializing SIPV Sovereign Environment..."

# 1. VERIFY BINARIES
OPENSSL_BIN=$(which openssl)
if [ -z "$OPENSSL_BIN" ]; then
    echo "[-] OpenSSL still missing. Forcing installation..."
    pkg install openssl -y
    hash -r
    OPENSSL_BIN=$(which openssl)
fi

# 2. ARCHITECTURE SETUP
mkdir -p $HOME/.vault_core
chmod 700 $HOME/.vault_core

# 3. KEY GENERATION (Using absolute path for stability)
if [ ! -f "$HOME/.vault_witness.key" ]; then
    $OPENSSL_BIN genpkey -algorithm ED25519 -out "$HOME/.vault_witness.key"
    $OPENSSL_BIN pkey -in "$HOME/.vault_witness.key" -pubout -out "$HOME/.vault_witness.pub"
    chmod 600 "$HOME/.vault_witness.key"
    echo "[+] Witness Keys Generated."
fi

if [ ! -f "$HOME/.vault_salt" ]; then
    $OPENSSL_BIN rand -hex 32 > "$HOME/.vault_salt"
    chmod 600 "$HOME/.vault_salt"
    echo "[+] Entropy Salt Generated."
fi

# 4. DEPLOY THE MASTER CLI
cat << 'INNER' > $HOME/sipv.sh
#!/data/data/com.termux/files/usr/bin/bash
VAULT_DIR="$HOME/.vault_core"

case "$1" in
    init)   ./cast.sh ;;
    stash)  ./vault.sh stash "$2" && ./oracle_engine.sh seal && ./uplink.sh ;;
    get)    ./vault.sh retrieve "$2" && ./oracle_engine.sh seal ;;
    audit)  ./audit.sh ;;
    roast)  echo "ROAST" | nc -w 1 127.0.0.1 50505 && echo "[!] Vault is cold." ;;
    status)
        pgrep -f sipv_oracle > /dev/null && echo "[+] Oracle: ONLINE" || echo "[-] Oracle: OFFLINE"
        echo "[*] Bucket: $(am get-standby-bucket com.termux)"
        ;;
    *) echo "Usage: sipv [init|stash|get|audit|roast|status]" ;;
esac
INNER

chmod +x $HOME/sipv.sh
ln -sf $HOME/sipv.sh $PREFIX/bin/sipv

echo "[+] SIPV Omni-Setup Complete. The "dental" is officially in the air."
