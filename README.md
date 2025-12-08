# Vision with Jetson Development Boards

<a href="LICENSE" ><img src="https://img.shields.io/github/license/quantumxt/cv4-jet-nano?style=for-the-badge"/></a>

This serves as a documentation & reference for the various resources available for the Jetson Orin Nano, Jetson Xavier NX & Jetson Nano development board with regards to Vision and AI based projects.

> The documentation/resources related to the older Jetson Nano (B01) / Jetson Xavier NX development boards would be shifted to the `archive` directory & the README [here](./archive/README_ARCHIVE.md).

> Documentation is tested on Jetpack 6.2 (`R36 (release), REVISION: 4.4`), you could check the system version via `cat /etc/nv_tegra_release`.

# Note: DO NOT upgrade to R36.4.7 (yet) if you intend to use the LLM!
> Update: 06/12/2025

The the new `R36.4.7` kernel update may [cause the model to not load properly due to a large memory allocation](https://forums.developer.nvidia.com/t/unable-to-allocate-cuda0-buffer-after-updating-ubuntu-packages/347862/69), in which the Jetson Orin Nano would not be able to handle. You can checkout the current kernel version via `cat /etc/nv_tegra_release`.

> If you have already updated to `R36.4.7`, there's another workaround whereby you have to [free up the RAM manually](https://forums.developer.nvidia.com/t/unable-to-allocate-cuda0-buffer-after-updating-ubuntu-packages/347862/138) to get the model to load.

The [current workaround for now](https://forums.developer.nvidia.com/t/unable-to-allocate-cuda0-buffer-after-updating-ubuntu-packages/347862/122) is to freeze the following kernel pacakges to `36.4.4`.

* nvidia-l4t-display-kernel
* nvidia-l4t-kernel
* nvidia-l4t-kernel-dtbs
* nvidia-l4t-kernel-headers
* nvidia-l4t-kernel-oot-headers
* nvidia-l4t-kernel-oot-modules 

```sh
sudo apt-mark hold nvidia-l4t-display-kernel nvidia-l4t-kernel nvidia-l4t-kernel-dtbs nvidia-l4t-kernel-headers nvidia-l4t-kernel-oot-headers nvidia-l4t-kernel-oot-modules 
```

**References**
- [https://forums.developer.nvidia.com/t/unable-to-allocate-cuda0-buffer-after-updating-ubuntu-packages/347862](https://forums.developer.nvidia.com/t/unable-to-allocate-cuda0-buffer-after-updating-ubuntu-packages/347862)

## Organisation
The directory is organised as follows:
- **AI_ML:** AI & Machine Learning related project(s)
- **cpp_proj:** C++ related projects
- **scripts:** Installation & setup scripts. Scripts for the older Jetson Nano and Jetson Xavier NX would be shifted to `archive` directory inside.

### Nvidia lib location
- libargus/Argus (For image processing): `/usr/src/jetson_multimedia_api/argus/`
- Multimedia API: `/usr/src/jetson_multimedia_api/`

## Hardware

The following components were used for this documentation:

- SBC: Nvidia Jetson Orin Nano Developer Kit (8GB)
- NVMe Storage: Transcend PCIe SSD 255S (1TB)

## Prerequisite

### pip

Install python package manager `pip`.

```sh
sudo apt install python3-pip
```

### CUDA paths

Ensure that the CUDA compiler (nvcc) is added to `~/.bashrc`, which could be done so via the `add_cuda_path.sh` script.

```sh
cd ~/cv4-jet-nano/scripts
sudo chmod +x add_cuda_path.sh
./add_cuda_path.sh
```

#### `jetson-containers` (For Language Models)

The `jetson-containers` would be used to run the various types of language models, such as LLM, VLM, etc.

```sh
git clone https://github.com/dusty-nv/jetson-containers
bash jetson-containers/install.sh
```

## Camera

### Setup (CSI Camera)

Configure the camera via the `jetson-io.py` script for the Jetson Orin Nano.

```sh
sudo /opt/nvidia/jetson-io/jetson-io.py
```

The terminal show display the menu. Select `Configure Jetson 24pin CSI Connector`.

```sh
  =================== Jetson Expansion Header Tool ===================
 |                                                                    |
 |                                                                    |
 |                    Select one of the following:                    |
 |                                                                    |
 |                   Configure Jetson 40pin Header                    |
 |                Configure Jetson 24pin CSI Connector                |
 |                  Configure Jetson M.2 Key E Slot                   |
 |                                Exit                                |
 |                                                                    |
 |====================================================================|
```

Select `Configure for compatible hardware`.

```sh
  =================== Jetson Expansion Header Tool ===================
 |                                                                    |
 |                                                                    |
 |                      3.3V (  1) .. (  2) i2c3                      |
 |                      i2c3 (  3) .. (  4) GND                       |
 |                       GND (  7) .. (  8) NA                        |
 |                        NA (  9) .. ( 10) GND                       |
 |                       GND ( 13) .. ( 14) NA                        |
 |                        NA ( 15) .. ( 16) GND                       |
 |                       GND ( 19) .. ( 20) NA                        |
 |                        NA ( 21) .. ( 22) GND                       |
 |                       GND ( 23) .. ( 24) GND                       |
 |                                                                    |
 |                                                                    |
 |                    Jetson 24pin CSI Connector:                     |
 |                                                                    |
 |                 Configure for compatible hardware                  |
 |                                Back                                |
 |====================================================================|
```

After that, select the camera model that you would be using.

> For example, if you are using a single IMX219 camera, select the `IMX219-A` option, which configures CAM0 for `IMX219-A` camera.

```sh
  =================== Jetson Expansion Header Tool ===================
 |                                                                    |
 |                                                                    |
 |                Select one of the following options:                |
 |                                                                    |
 |                         Camera IMX219 Dual                         |
 |                          Camera IMX219-A                           |
 |                    Camera IMX219-A and IMX477-C                    |
 |                          Camera IMX219-C                           |
 |                         Camera IMX477 Dual                         |
 |                     Camera IMX477 Dual 4 lane                      |
 |                          Camera IMX477-A                           |
 |                    Camera IMX477-A and IMX219-C                    |
 |                          Camera IMX477-C                           |
 |                                                                    |
 |                                Back                                |
 |                                                                    |
 |                                                                    |
 |====================================================================|
```

Select `Save pin changes`.

```sh
  =================== Jetson Expansion Header Tool ===================
 |                                                                    |
 |                                                                    |
 |                      3.3V (  1) .. (  2) i2c3                      |
 |                      i2c3 (  3) .. (  4) GND                       |
 |                       GND (  7) .. (  8) NA                        |
 |                        NA (  9) .. ( 10) GND                       |
 |                       GND ( 13) .. ( 14) NA                        |
 |                        NA ( 15) .. ( 16) GND                       |
 |                       GND ( 19) .. ( 20) NA                        |
 |                        NA ( 21) .. ( 22) GND                       |
 |                       GND ( 23) .. ( 24) GND                       |
 |                                                                    |
 |                                                                    |
 |                    Jetson 24pin CSI Connector:                     |
 |                                                                    |
 |                          Save pin changes                          |
 |                        Discard pin changes                         |
 |====================================================================|
```

Reboot the Jetson to use the camera!

**References**

- [https://forums.developer.nvidia.com/t/help-with-imx219-cameras/290628](https://forums.developer.nvidia.com/t/help-with-imx219-cameras/290628)
- [https://forums.developer.nvidia.com/t/no-cameras-available/320541/2](https://forums.developer.nvidia.com/t/no-cameras-available/320541/2)
- [https://docs.arducam.com/Nvidia-Jetson-Camera/Application-note/Jetson-io/](https://docs.arducam.com/Nvidia-Jetson-Camera/Application-note/Jetson-io/)

### First image

Use the `nvgstcapture-1.0` command to check whether the camera is working.

```sh
nvgstcapture-1.0
nvgstcapture-1.0 --orientation 2	# Rotate image output by 180 degrees
```

**References**

- [https://developer.nvidia.com/embedded/learn/tutorials/first-picture-csi-usb-camera](https://developer.nvidia.com/embedded/learn/tutorials/first-picture-csi-usb-camera)

## Running LLM/AI model

### Prerequistes

As the Jetson Orin Nano Developer kit only has 8GB ram, it may not be enough to run the AI models. To compensate for the lack of memory, swap files or partitions could be created in the NVMe SSD. (If you are using a Micro SD card, this could also be done, but it would not be as fast as NVMe SSD)

#### Check current space allocation

Check the current swap size.

```sh
free -h
```

The output should be as follows:

```sh
total        used        free      shared  buff/cache   available
Mem:           7.4Gi       2.7Gi       3.0Gi        67Mi       1.7Gi       4.7Gi
Swap:          3.7Gi          0B       3.7Gi
```

Check the current available space via `df` tool.

```sh
df -h
```

An example layout is shown below.

```sh
ubuntu@ubuntu:~$ df -h
Filesystem       Size  Used Avail Use% Mounted on
/dev/nvme0n1p1   915G   19G  850G   3% /
tmpfs            3.8G  172K  3.8G   1% /dev/shm
tmpfs            1.5G   35M  1.5G   3% /run
tmpfs            5.0M  4.0K  5.0M   1% /run/lock
/dev/nvme0n1p10   63M  110K   63M   1% /boot/efi
tmpfs            762M  188K  762M   1% /run/user/1000
```

#### Creating the swap file

Create a swap file.

```sh
sudo fallocate -l 16G /swapfile
```

> **Note:** Check the size of the swap file via `ls -lh /swapfile`

Update the permission and enable the swap parition.

```sh
sudo chmod 600 /swapfile    # Ensure only root/sys has access
sudo mkswap /swapfile
sudo swapon /swapfile
```

Check if the swap size has increased by 16GB.

```sh
free -h
```

The output should be as follows:
```sh
               total        used        free      shared  buff/cache   available
Mem:           7.4Gi       2.7Gi       3.0Gi        70Mi       1.7Gi       4.7Gi
Swap:           19Gi          0B        19Gi
```

**References**
- [https://www.forecr.io/blogs/programming/how-to-increase-swap-space-on-jetson-modules](https://www.forecr.io/blogs/programming/how-to-increase-swap-space-on-jetson-modules)

### Test run

#### SLM

Run a SLM model, where the image would be pulled from `dustynv` repo.

```sh
jetson-containers run $(autotag nano_llm) \
  python3 -m nano_llm.chat --api=mlc \
    --model TinyLlama/TinyLlama-1.1B-Chat-v1.0
```

> The `dustynv/nano_llm:r36.4.0` container is `12.7GB`.

After pulling the image, a prompt terminal would be displayed.

```sh
┌─────────────────────────┬─────────────────────────────────────────────────────────────────────────────┐
│ architectures           │ ['LlamaForCausalLM']                                                        │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ attention_bias          │ False                                                                       │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ bos_token_id            │ 1                                                                           │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ eos_token_id            │ 2                                                                           │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ hidden_act              │ silu                                                                        │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ hidden_size             │ 2048                                                                        │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ initializer_range       │ 0.02                                                                        │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ intermediate_size       │ 5632                                                                        │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ max_position_embeddings │ 2048                                                                        │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ model_type              │ llama                                                                       │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ num_attention_heads     │ 32                                                                          │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ num_hidden_layers       │ 22                                                                          │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ num_key_value_heads     │ 4                                                                           │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ pretraining_tp          │ 1                                                                           │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ rms_norm_eps            │ 1e-05                                                                       │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ rope_scaling            │                                                                             │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ rope_theta              │ 10000.0                                                                     │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ tie_word_embeddings     │ False                                                                       │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ torch_dtype             │ bfloat16                                                                    │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ transformers_version    │ 4.35.0                                                                      │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ use_cache               │ True                                                                        │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ vocab_size              │ 32000                                                                       │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ name                    │ TinyLlama-1.1B-Chat-v1.0                                                    │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ api                     │ mlc                                                                         │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_projector_path       │ /data/models/huggingface/models--TinyLlama--TinyLlama-1.1B-Chat-v1.0/snapsh │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ quant                   │ q4f16_ft                                                                    │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ type                    │ llama                                                                       │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ max_length              │ 2048                                                                        │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ prefill_chunk_size      │ -1                                                                          │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ load_time               │ 126.91317226499996                                                          │
├─────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ params_size             │ 533.740234375                                                               │
└─────────────────────────┴─────────────────────────────────────────────────────────────────────────────┘

00:25:18 | INFO | using chat template 'tiny-llama' for model TinyLlama-1.1B-Chat-v1.0
00:25:18 | INFO | model 'TinyLlama-1.1B-Chat-v1.0', chat template 'tiny-llama' stop tokens:  ['</s>'] -> [2]
>> PROMPT:
```

#### VLM

We'll be testing the `Llava-v1.6-7B` VLM model.

```sh
jetson-containers run $(autotag nano_llm) python3 -m nano_llm.chat --api=mlc --model Efficient-Large-Model/VILA1.5-3b --max-context-len 256  --max-new-tokens 512  --prompt '/data/images/lake.jpg'  --prompt 'Describe the scene'
```

The output should look something similar.

```sh
...
22:58:06 | INFO | mm_projector (mlp_downsample)  Sequential(
  (0): DownSampleBlock()
  (1): LayerNorm((4608,), eps=1e-05, elementwise_affine=True)
  (2): Linear(in_features=4608, out_features=2560, bias=True)
  (3): GELU(approximate='none')
  (4): Linear(in_features=2560, out_features=2560, bias=True)
)
22:58:06 | INFO | mm_projector weights:  dict_keys(['1.bias', '1.weight', '2.bias', '2.weight', '4.bias', '4.weight'])
┌────────────────────────────┬─────────────────────────────────────────────────────────────────────────────┐
│ _name_or_path              │ ./llm                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ architectures              │ ['LlamaForCausalLM']                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ drop_path_rate             │ 0.0                                                                         │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ hidden_size                │ 2560                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ image_aspect_ratio         │ resize                                                                      │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ interpolate_mode           │ linear                                                                      │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_hidden_size             │ 1152                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_projector_lr            │                                                                             │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_use_im_patch_token      │ False                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_use_im_start_end        │ False                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_vision_select_feature   │ cls_patch                                                                   │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_vision_select_layer     │ -2                                                                          │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ model_dtype                │ torch.bfloat16                                                              │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ model_type                 │ llama                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ num_video_frames           │ 8                                                                           │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ resume_path                │ ./vlm                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ s2                         │ False                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ s2_max_split_size          │ 336                                                                         │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ s2_scales                  │ 336,672,1008                                                                │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ transformers_version       │ 4.36.2                                                                      │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ tune_language_model        │ True                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ tune_mm_projector          │ True                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ tune_vision_tower          │ True                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ vision_resolution          │ -1                                                                          │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ name                       │ VILA1.5-3b                                                                  │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ api                        │ mlc                                                                         │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ max_position_embeddings    │ 4096                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_vision_tower            │ /data/models/huggingface/models--Efficient-Large-Model--VILA1.5-3b/snapshot │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_projector_path          │ /data/models/huggingface/models--Efficient-Large-Model--VILA1.5-3b/snapshot │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ mm_projector_type          │ mlp_downsample                                                              │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ attention_bias             │ False                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ attention_dropout          │ 0.0                                                                         │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ bos_token_id               │ 1                                                                           │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ eos_token_id               │ 2                                                                           │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ hidden_act                 │ silu                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ initializer_range          │ 0.02                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ intermediate_size          │ 6912                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ model_max_length           │ 4096                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ num_attention_heads        │ 20                                                                          │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ num_hidden_layers          │ 32                                                                          │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ num_key_value_heads        │ 20                                                                          │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ pad_token_id               │ 0                                                                           │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ pretraining_tp             │ 1                                                                           │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ rms_norm_eps               │ 1e-05                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ rope_scaling               │                                                                             │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ rope_theta                 │ 10000.0                                                                     │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ tie_word_embeddings        │ False                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ tokenizer_model_max_length │ 4096                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ tokenizer_padding_side     │ right                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ torch_dtype                │ bfloat16                                                                    │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ use_cache                  │ True                                                                        │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ vocab_size                 │ 32000                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ quant                      │ q4f16_ft                                                                    │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ type                       │ llama                                                                       │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ max_length                 │ 256                                                                         │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ prefill_chunk_size         │ -1                                                                          │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ load_time                  │ 12.812568535000537                                                          │
├────────────────────────────┼─────────────────────────────────────────────────────────────────────────────┤
│ params_size                │ 1300.8330078125                                                             │
└────────────────────────────┴─────────────────────────────────────────────────────────────────────────────┘

22:58:06 | INFO | using chat template 'vicuna-v1' for model VILA1.5-3b
22:58:06 | INFO | model 'VILA1.5-3b', chat template 'vicuna-v1' stop tokens:  ['</s>'] -> [2]
>> PROMPT: /data/images/lake.jpg

>> PROMPT: Describe the scene

The scene is a serene and picturesque view of a wooden pier extending into a calm and reflective lake. The pier, appearing to be made of weathered wood, leads the eye towards the distant mountains that form the backdrop of this tranquil setting. The mountains, partially shrouded in mist, add a sense of depth and mystery to the scene. The sky above is overcast, casting a soft and diffused light over the entire landscape. The colors in the image are predominantly shades of blue and green, reflecting the natural elements of water, sky, and foliage. The overall mood of the image is peaceful and contemplative, inviting the viewer to pause and appreciate the beauty of nature.</s>

┌───────────────┬────────────┐
│ embed_time    │ 0.00034565 │
├───────────────┼────────────┤
│ input_tokens  │    244     │
├───────────────┼────────────┤
│ output_tokens │    155     │
├───────────────┼────────────┤
│ prefill_time  │  0.326017  │
├───────────────┼────────────┤
│ prefill_rate  │  748.427   │
├───────────────┼────────────┤
│ decode_time   │  5.00109   │
├───────────────┼────────────┤
│ decode_rate   │  30.7933   │
└───────────────┴────────────┘
```

**References**
- [https://www.jetson-ai-lab.com/tutorial_nano-llm.html](https://www.jetson-ai-lab.com/tutorial_nano-llm.html)
- [https://www.jetson-ai-lab.com/tutorial_nano-vlm.html](https://www.jetson-ai-lab.com/tutorial_nano-vlm.html)

## Tools

### jtop

Install `jtop` via `pip`.

```sh
sudo pip install -U jetson-stats
```

Reboot the jetson to use `jtop`.

```sh
jtop
```

### Firefox

Install Firefox browser via `flatpak`, as there may be issues installing via snap.

```sh
cd ~/cv4-jet-nano/scripts
sudo chmod +x install_firefox_flatpak.sh
./install_firefox_flatpak.sh
```

 **References**
 - [https://jetsonhacks.com/2025/07/12/why-chromium-suddenly-broke-on-jetson-orin-and-how-to-bring-it-back/](https://jetsonhacks.com/2025/07/12/why-chromium-suddenly-broke-on-jetson-orin-and-how-to-bring-it-back/)
 - [https://forums.developer.nvidia.com/t/neither-chromium-nor-firefox-work-with-my-jetson-orin-nano/338669](https://forums.developer.nvidia.com/t/neither-chromium-nor-firefox-work-with-my-jetson-orin-nano/338669)
 
### Docker

Ensure that docker is installed first.

```sh
sudo apt update
sudo apt install -y nvidia-container
```

Access docker without running `sudo`, by adding user to the `docker` group.

```sh
cd ~/cv4-jet-nano/scripts
sudo chmod +x add_docker_group.sh
./add_docker_group.sh
```

## Troubleshooting

### `jetson-containers` not found

#### Check Jetpack installation

Check if `Jetpack` is installed via `dpkg -l | grep nvidia-jetpack`. If it returns nothing, it means that the `Jetpack` installation is missing.

Install `nvidia-jetpack`.

```sh
sudo apt update
sudo apt install -y nvidia-jetpack
```

After the installation, run `dpkg -l | grep nvidia-jetpack` to check the installation. It should print something similar as shown below.

```sh
ii  nvidia-jetpack                          6.2.1+b38                 arm64        NVIDIA Jetpack Meta Package
ii  nvidia-jetpack-dev                      6.2.1+b38                 arm64        NVIDIA Jetpack dev Meta Package
ii  nvidia-jetpack-runtime                  6.2.1+b38                 arm64        NVIDIA Jetpack runtime Meta Package
```

**References**

- [https://github.com/dusty-nv/jetson-containers](https://github.com/dusty-nv/jetson-containers)

### ImportError: libnvdla_compiler.so: cannot open shared object file: No such file or directory

The `libnvdla_compiler.so` library is not included in the firmware `R36.4.4`, so the fix is to grab the file from the .deb file from the previous version.

```sh
wget -O compiler.deb https://repo.download.nvidia.com/jetson/common/pool/main/n/nvidia-l4t-dla-compiler/nvidia-l4t-dla-compiler_36.4.1-20241119120551_arm64.deb | dpkg-deb --fsys-tarfile compiler.deb | sudo tar xv --strip-components=5 --directory=/usr/lib/aarch64-linux-gnu/nvidia/ ./usr/lib/aarch64-linux-gnu/nvidia/libnvdla_compiler.so
```

If nothing seems to be happening after the file has been download, `sudo` may be waiting for you to enter your password. The command above would download the previous version compiler, and extract the `libnvdla_compiler.so` file into the proper location in `/usr/lib/aarch64-linux-gnu/nvidia/`. For more info, checkout the gist below.

**Credits**
- [https://gist.github.com/jasongill/fef55cb769657bf36d0bb20ada742249](https://gist.github.com/jasongill/fef55cb769657bf36d0bb20ada742249)


### "Jetpack not installed" when using `jtop`

`jtop` might not be updated to display the latest Jetpack version, we could use a patch from [jetsonhacks](https://github.com/jetsonhacks/jetson-jtop-patch) to fix this issue for now.

```sh
git clone https://github.com/jetsonhacks/jetson-jtop-patch.git
cd jetson-jtop-patch
chmod +x apply_jtop_fix.sh
./apply_jtop_fix.sh
```

Reboot after the patch has been applied.

```sh
sudo reboot
```

The Jetpack version should be shown when running `jtop` again.

```sh
jtop
```

**References**
- Main: https://github.com/jetsonhacks/jetson-jtop-patch
- Alternative: https://github.com/cyaninfinite/jetson-jtop-patch

### Entering terminal session

To enter into terminal session inside the GUI (Gnome Desktop).

```sh
sudo init 3
```

To return to GUI:

```sh
sudo init 5
```

## Archive (Jetson Nano B01 / Jetson Xavier NX)

For documentation/testing of the Jetson Nano B01 / Jetson Xavier NX, checkout the README [here](./archive/README_ARCHIVE.md).

# License
Licensed under the [MIT License](./LICENSE).

