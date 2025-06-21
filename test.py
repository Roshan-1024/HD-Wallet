from bip_utils import Bip39SeedGenerator, Bip44, Bip44Coins, Bip44Changes

def derive_bsc_keys(seed_hex: str):
    # 1. Generate seed bytes from hex
    seed_bytes = bytes.fromhex(seed_hex)

    # 2. Initialize BIP44 for BSC (same as Ethereum)
    bip44_mst = Bip44.FromSeed(seed_bytes, Bip44Coins.ETHEREUM)

    # 3. Derive the final address key: m/44'/60'/0'/0/0
    bip44_acc = bip44_mst.Purpose().Coin().Account(0)
    bip44_chg = bip44_acc.Change(Bip44Changes.CHAIN_EXT)
    bip44_addr = bip44_chg.AddressIndex(0)

    # 4. Get the keys
    private_key = bip44_addr.PrivateKey().Raw().ToHex()
    public_key = bip44_addr.PublicKey().RawCompressed().ToHex()  # compressed pubkey

    print("Final Private Key:", private_key)
    print("Final Public Key (compressed):", public_key)

# Example usage
if __name__ == "__main__":
    # Example seed from BIP-39 (32-byte hex string, not the mnemonic)
    seed = input("Enter your seed (hex): ").strip()
    derive_bsc_keys(seed)
