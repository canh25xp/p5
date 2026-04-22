# Helix C/C++ client API (P4API) notes

## `ClientAPI::SetProtocol(p, v)`

`SetProtocol` sets **Helix protocol variables** before the connection is established. It must be called **before** `ClientAPI::Init()`.

The first string is the variable name; the second is its value. For flags that are only on/off, the value is often an empty string.

### Documented variables

| Name         | Meaning |
| ------------ | ------- |
| `tag`        | Request **tagged** output when the server supports it for the command. Triggers `ClientUser::OutputStat()` instead of `OutputInfo()` for that data. |
| `specstring` | Enable **specstring** / form-descriptor handling for form-related commands. Commonly used together with `tag`. |
| `api`        | Set the **protocol level** (e.g. `"55"`, `"56"`, `"57"`, `"58"`, …) to pin older client/server response behavior. Normally matches the API you linked; override only if you need compatibility with a specific release line. |

Example (tagged output, same idea as `p4 -z tag`):

```cpp
client.SetProtocol("tag", "");
client.Init(&e);
```

Example (forms with specstring + tag, as in the P4API user guide):

```cpp
client.SetProtocol("specstring", "");
client.SetProtocol("tag", "");
client.Init(&e);
```

Official reference: [ClientApi::SetProtocol](https://www.perforce.com/manuals/v15.1/p4api/chapter.methods.html#clientapi.setprotocol) (content is carried forward in current Helix P4API docs).

### Other protocol keys

The `p4` client’s **`-z name`** and **`-z name=value`** options set protocol variables for the session. The table above is the small **documented** core. Newer server releases may add more variables; if the server accepts a name, you can set it the same way via `SetProtocol` before `Init()`.

In **p5**, global **`-z`** is parsed into name/value pairs and applied with `SetProtocol` during initialization (see `apply_protocol_options` and `P5::Initialize`).
