import base64
from cryptography.hazmat.primitives.asymmetric import ec

# 1. Generate an Elliptic Curve keypair on the SECP224R1 curve (used by Apple Find My)
private_key = ec.generate_private_key(ec.SECP224R1())

# 2. Extract the private key (28 bytes)
priv_num = private_key.private_numbers().private_value
priv_bytes = priv_num.to_bytes(28, 'big')
priv_b64 = base64.b64encode(priv_bytes).decode('utf-8')

# 3. Extract the public key's X-coordinate (28 bytes)
pub_num = private_key.public_key().public_numbers()
pub_x_bytes = pub_num.x.to_bytes(28, 'big')
pub_b64 = base64.b64encode(pub_x_bytes).decode('utf-8')

print("=" * 60)
print("SAVE THESE KEYS SAFELY:")
print("=" * 60)
print(f"Private Key (Base64)     : {priv_b64}")
print(f"Advertisement Key (Base64): {pub_b64}")
print("-" * 60)
print("Paste this byte array into your Arduino sketch:")
c_array = ", ".join(f"0x{b:02X}" for b in pub_x_bytes)
print(f"uint8_t public_key[28] = {{ {c_array} }};")
print("=" * 60)
